extern crate serde_derive;
#[macro_use]
extern crate actix;

extern crate log;

mod analysis;
mod codec;

use crate::analysis::{Init, Data};
use protobuf::{
    CodedOutputStream, Message, RepeatedField
};

use tokio_codec::FramedRead;
use tokio_io::AsyncRead;
use tokio_tcp::TcpStream;

use std::str::FromStr;
use serde::{Deserialize, Serialize};
//use std::time::{Duration, Instant};
use actix::prelude::*;
use actix_files as fs;
use actix_web::{
    middleware, web, App, Error, HttpRequest, HttpResponse, HttpServer
};

use actix_web_actors::ws;
use actix::Context;

use std::net::{
    ToSocketAddrs, IpAddr, SocketAddr
};
use std::io;

// How often heartbeat pings are sent
//const HEARTBEAT_INTERVAL: Duration = Duration::from_secs(5);
// How long before lack of client response causes a timeout
//const CLIENT_TIMEOUT: Duration = Duration::from_secs(10);

/// do websocket handshake and start `MyWebSocket` actor
fn ws_index(r: HttpRequest, stream: web::Payload) -> Result<HttpResponse, Error> {
    println!("Calling main function");
    //println!("{:?}", r);
    let res = ws::start(Ws::new(), &r, stream);
    //println!("{:?}", res.as_ref().unwrap());
    res
}

#[derive(Serialize, Deserialize, Debug)]
struct CFreqItem {
    name: String,
    freq: f64
}

#[derive(Serialize, Deserialize, Debug)]
struct CFreq {
    cfreq: Vec<CFreqItem>
}

#[derive(Serialize, Deserialize, Debug)]
struct Params {
    #[serde(skip_serializing_if = "Option::is_none")]
    avg: ::std::option::Option<f64>,
    #[serde(skip_serializing_if = "Option::is_none")]
    min: ::std::option::Option<f64>,
    #[serde(skip_serializing_if = "Option::is_none")]
    max: ::std::option::Option<f64>,
    #[serde(skip_serializing_if = "Option::is_none")]
    client: ::std::option::Option<i32>
}

#[derive(Serialize, Deserialize, Debug)]
#[serde(tag = "type")]
enum Msg {
    Init { 
        net_int: Vec<String>,
        interval: u32,
        duration: u32,
        //top_client_cnt: u32,
        active_metric: u32,
        client_ip: ::std::option::Option<u32>,
        client_port: ::std::option::Option<u32>,
        server_ip: ::std::option::Option<u32>,
        server_port: ::std::option::Option<u32>
    },
    Data {
        time: i64,
        net_int: String,
        #[serde(skip_serializing_if = "Option::is_none")]
        rst: ::std::option::Option<Params>,                
        #[serde(skip_serializing_if = "Option::is_none")]
        err_rate: ::std::option::Option<Params>,
        #[serde(skip_serializing_if = "Option::is_none")]
        req_rate: ::std::option::Option<Params>,
        #[serde(skip_serializing_if = "Option::is_none")]
        tp: ::std::option::Option<Params>,
        #[serde(skip_serializing_if = "Option::is_none")]
        tp_rev: ::std::option::Option<Params>,
        #[serde(skip_serializing_if = "Option::is_none")]
        conn_rate: ::std::option::Option<Params>,
        #[serde(skip_serializing_if = "Option::is_none")]
        client: ::std::option::Option<CFreq>,
        #[serde(skip_serializing_if = "Option::is_none")]
        req_path: ::std::option::Option<CFreq>,
        #[serde(skip_serializing_if = "Option::is_none")]
        req_method: ::std::option::Option<CFreq>,
        #[serde(skip_serializing_if = "Option::is_none")]
        req_type: ::std::option::Option<CFreq>,
        #[serde(skip_serializing_if = "Option::is_none")]
        rsp_status: ::std::option::Option<CFreq>
    }
}

struct Ws {
    // Client must send ping at least once per 10 seconds, otherwise we drop connection.
    //hb: Instant
}

impl Ws {
    fn new() -> Self {
        //Self { hb: Instant::now() }
        Self {}
    }

    // helper method that sends ping to client every second.
    // also this method checks heartbeats from client
    /*fn hb(&self, ctx: &mut <Self as Actor>::Context) {
        ctx.run_interval(HEARTBEAT_INTERVAL, |act, ctx| {
            // check client heartbeats
            if Instant::now().duration_since(act.hb) > CLIENT_TIMEOUT {
                // heartbeat timed out
                println!("Websocket Client heartbeat failed, disconnecting!");

                // stop actor
                ctx.stop();

                // don't try to send a ping
                return;
            }

            ctx.ping("");
        });
    }*/
}


impl Actor for Ws {
    type Context = ws::WebsocketContext<Self>;

    fn started(&mut self, _ctx: &mut Self::Context) {
        println!("WebSocket actor started");

        // we'll start heartbeat process on session start.
        //self.hb(ctx);
    }

    fn stopping(&mut self, _: &mut Self::Context) -> Running {
        println!("WebSocket Actor closed");
        Running::Stop
    }
}

/// Handler for `ws::Message`
impl StreamHandler<ws::Message, ws::ProtocolError> for Ws {
    fn started(&mut self, _ctx: &mut Self::Context) {
        println!("WebSocket session openned");
    }

    fn finished(&mut self, _ctx: &mut Self::Context) {
        println!("WebSocket session closed");
    }

    fn handle(&mut self, msg: ws::Message, ctx: &mut Self::Context) {
        // process websocket messages
        match msg {
            ws::Message::Ping(_) => {
            }
            ws::Message::Pong(_) => {
            }
            ws::Message::Text(text) => {
                //println!("WS: {:?}", text);
                
                let m: Msg = match serde_json::from_str(&text){
                    Result::Ok(val) => {val},
                    Result::Err(err) => {panic!("Unable to parse json: {}",err)}
                };

                println!("{:?}", m);

                match m {
                    Msg::Init { net_int, interval, duration, active_metric, client_ip, client_port, server_ip, server_port } => {
                        let mut init = Init::new();
                        init.set_interval(interval);
                        init.set_duration(duration);
                        init.set_activeMetric(active_metric);
                        
                        match client_ip {
                            Some(c_ip) => {
                                init.set_clientIP(c_ip);

                                match client_port {
                                    Some(c_port) => {
                                        init.set_clientPort(c_port);                                        
                                    },
                                    _ => {}
                                }
                            },
                            _ => {}
                        }

                        match server_ip {
                            Some(s_ip) => {
                                init.set_serverIP(s_ip);

                                match server_port {
                                    Some(s_port) => {
                                        init.set_serverPort(s_port);                                        
                                    },
                                    _ => {}
                                }
                            },
                            _ => {}
                        }
                        
                        let net_int = RepeatedField::from_vec(net_int);
                        init.set_netInt(net_int);

                        println!("{:?}", init);

                        let ws_addr: Addr<_> = ctx.address();
                        
                        //let ip = resolve("bmj-cluster.cs.mcgill.ca:15430");
                        SnifferServer::connect("127.0.0.1:3000", init, ws_addr);
                    }
                    Msg::Data { time: _, net_int: _, rst: _, tp: _, tp_rev: _, err_rate: _, req_rate: _, conn_rate: _, client: _, req_path: _, req_method: _, req_type: _, rsp_status: _ } => println!("Got Data")
                }
            }

            ws::Message::Binary(_) => println!("Unexpected binary"),
            ws::Message::Close(_) => {
                ctx.stop();
            }
            ws::Message::Nop => (),
        }
    }
}

impl Handler<Data> for Ws {
    type Result = ();

    fn handle(&mut self, data: Data, ctx: &mut Self::Context) {
        let msg = Msg::Data {
            time: data.get_time(),
            net_int: data.get_netInt().to_string(),
            rst: match data.has_rstAvg() {
                true => Some(Params { 
                    avg: Some(data.get_rstAvg()),
                    min: Some(data.get_rstMin()),
                    max: Some(data.get_rstMax()),
                    client: None
                }),
                _ => None
            },
            tp: match data.has_tpAvg() {
                true => Some(Params { 
                    avg: Some(data.get_tpAvg()),
                    min: Some(data.get_tpMin()),
                    max: Some(data.get_tpMax()),
                    client: None
                }),
                _ => None
            },
            tp_rev: match data.has_tpRevAvg() {
                true => Some(Params { 
                    avg: Some(data.get_tpRevAvg()),
                    min: Some(data.get_tpRevMin()),
                    max: Some(data.get_tpRevMax()),
                    client: None
                }),
                _ => None
            },
            err_rate: match data.has_errRate() {
                true => Some(Params { 
                    avg: Some(data.get_errRate()),
                    min: Some(data.get_errRateMin()),
                    max: Some(data.get_errRateMax()),
                    client: None
                }),
                _ => None
            },
            req_rate: match data.has_reqRate() {
                true => Some(Params { 
                    avg: Some(data.get_reqRate()),
                    min: Some(data.get_reqRateMin()),
                    max: Some(data.get_reqRateMax()),
                    client: None 
                }),
                _ => None
            },
            conn_rate: match data.has_connRate() {
                true => Some(Params { 
                    avg: Some(data.get_connRate()),
                    min: Some(data.get_connRateMin()),
                    max: Some(data.get_connRateMax()),
                    client: None
                }),
                _ => None
            },
            client: {
                let mut v = Vec::new();
                for client in data.get_client() {
                    let cfi = CFreqItem {
                        name: client.get_name().to_string(),
                        freq: client.get_freq()
                    };
                    v.push(cfi);
                }

                let cf = CFreq {
                    cfreq: v
                };

                Some(cf)
            },
            req_path: {
                let mut v = Vec::new();
                for req_path in data.get_req_path() {
                    let cfi = CFreqItem {
                        name: req_path.get_name().to_string(),
                        freq: req_path.get_freq()
                    };
                    v.push(cfi);
                }    

                let cf = CFreq {
                    cfreq: v
                };

                Some(cf)
            },
            req_method: {
                let mut v = Vec::new();
                for req_method in data.get_req_method() {
                    let cfi = CFreqItem {
                        name: req_method.get_name().to_string(),
                        freq: req_method.get_freq()
                    };
                    v.push(cfi);
                }    

                let cf = CFreq {
                    cfreq: v
                };

                Some(cf)
            },
            req_type: {
                let mut v = Vec::new();
                for req_type in data.get_req_type() {
                    let cfi = CFreqItem {
                        name: req_type.get_name().to_string(),
                        freq: req_type.get_freq()
                    };
                    v.push(cfi);
                }    
                
                let cf = CFreq {
                    cfreq: v
                };

                Some(cf)
            },
            rsp_status: {
                let mut v = Vec::new();
                for rsp_status in data.get_rsp_status() {
                    let cfi = CFreqItem {
                        name: rsp_status.get_name().to_string(),
                        freq: rsp_status.get_freq()
                    };
                    v.push(cfi);
                }    
                
                let cf = CFreq {
                    cfreq: v
                };

                Some(cf)
            },
        };

        let serialized = serde_json::to_string(&msg).unwrap();
        ctx.text(serialized);

        /*ctx.text("{ 
            netInt:".to_string() + &data.get_netInt().to_string() + &",".to_string()
            + &"rst:".to_string() + &data.get_rst().to_string() + &",".to_string()
            + &"erate:".to_string() + &data.get_errRate().to_string() + &",".to_string()
            + &"}".to_string()
        );*/
    }
}

/// Define tcp server that will accept incoming tcp connection and create
/// chat actors.
struct SnifferServer {
    recipient: Recipient<Data>
}

fn resolve(host: &str) -> io::Result<Vec<IpAddr>> {
    (host, 0).to_socket_addrs().map(|iter| iter.map(|socket_address| socket_address.ip()).collect())
}

impl SnifferServer {
    pub fn connect(_s: &str, init: Init, ws_addr: Addr<Ws>) {
        // Create server listener
        let addr = SocketAddr::from_str(_s).unwrap();

        Arbiter::spawn(
            TcpStream::connect(&addr)
                .and_then(move |stream| {
                    let (r, mut w) = stream.split();
                    
                    //println!("Connected !!!");

                    let mut os = CodedOutputStream::new(&mut w);
                    init.write_length_delimited_to(&mut os);
                    os.flush();
                    
                    SnifferServer::create(|ctx| {
                        SnifferServer::add_stream(FramedRead::new(r, codec::SnifferCodec), ctx);
                        SnifferServer {
                            recipient: ws_addr.recipient()
                        }
                    });

                    futures::future::ok(())
                })
                .map_err(|e| {
                    println!("Can not connect to server: {}", e);
                })
        );
    }
}

/// Make actor from `Server`
impl Actor for SnifferServer {
    /// Every actor has to provide execution `Context` in which it can run.
    type Context = Context<Self>;

    fn started(&mut self, _ctx: &mut Self::Context) {
        println!("Sniffer Actor started!");        
    }
}

impl actix::Message for Data {
    type Result = ();
}

/// Handler for `ws::Message`
impl StreamHandler<Data, io::Error> for SnifferServer {
    
    fn handle(&mut self, data: Data, _ctx: &mut Context<Self>) { 
        if let Err(error) = self.recipient.do_send(data) {
            println!("do_send failed: {}", error);
        }
    }
}

fn main() -> io::Result<()> {
    std::env::set_var("RUST_LOG", "actix_server=info,actix_web=info");
    let _ = env_logger::init();

    //System is an actor which manages process.
    // Before starting any actix's actors, System actor has to be create with System::new() call. 
    // This method creates new Arbiter in current thread and starts System actor.
    let sys = actix::System::new("app-server");

    // Create Http server with websocket support
    HttpServer::new(|| {
        App::new()
            // enable logger
            .wrap(middleware::Logger::default())
            // websocket route
            .service(web::resource("/ws/").route(web::get().to(ws_index)))
            // static files
            .service(fs::Files::new("/", "static/").index_file("index.html"))
    })
    
    // start http server on 0.0.0.0:80
    .bind("127.0.0.1:8081")?
    .start();

    sys.run()
}