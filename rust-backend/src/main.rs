extern crate serde_derive;
#[macro_use]
extern crate actix;

extern crate log;

mod analysis;
mod codec;

use crate::analysis::{Init, Data, MetricMsg, MetricAvgMsg, MetricCumDistrMsg};
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
struct MetricAvg {
    avg: f64,
    min: f64,
    max: f64
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
    },

    Metric {
        name: String,    
        time: i64,
        net_int: String,
        client_id: i64,
        #[serde(skip_serializing_if = "Option::is_none")]
        metricAvg: std::option::Option<MetricAvg>,
        #[serde(skip_serializing_if = "Option::is_none")]
        metricCumDistr: std::option::Option<CFreq>
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
                    Msg::Data { time: _, net_int: _, rst: _, tp: _, tp_rev: _, err_rate: _, req_rate: _, conn_rate: _, client: _, req_path: _, req_method: _, req_type: _, rsp_status: _ } => println!("Got Data"),
                    _ => {}
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

impl Handler<MetricMsg> for Ws {
    type Result = ();

    fn handle(&mut self, metric: MetricMsg, ctx: &mut Self::Context) {
        let mut metricAvg = None;
        if(metric.has_metricAvg()) {
            let mAvg = metric.get_metricAvg();
            metricAvg = Some(MetricAvg {
                avg: mAvg.get_avg(),
                min: mAvg.get_min(),
                max: mAvg.get_max()
            });
        }
        
        let mut metricCumDistr = None;
        if(metric.has_metricCumDistr()) {  
            let mCumDistr = metric.get_metricCumDistr();
            let mut v = Vec::new();
            for freq in mCumDistr.get_freqs() {
                let cfi = CFreqItem {
                    name: freq.get_name().to_string(),
                    freq: freq.get_freq()
                };
                v.push(cfi);
            }

            let cf = CFreq {
                cfreq: v
            };

            metricCumDistr = Some(cf)
        }

        let msg = Msg::Metric {
            name: metric.get_name().to_string(),    
            time: metric.get_time(),
            net_int: metric.get_netInt().to_string(),
            client_id: 0,
            metricAvg: metricAvg,
            metricCumDistr: metricCumDistr
        };

        let serialized = serde_json::to_string(&msg).unwrap();

        ctx.text(serialized);
    }
}

/// Define tcp server that will accept incoming tcp connection and create
/// chat actors.
struct SnifferServer {
    recipient: Recipient<MetricMsg>
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

impl actix::Message for MetricMsg {
    type Result = ();
}

/// Handler for `ws::Message`
impl StreamHandler<MetricMsg, io::Error> for SnifferServer {
    
    fn handle(&mut self, metric: MetricMsg, _ctx: &mut Context<Self>) { 
        if let Err(error) = self.recipient.do_send(metric) {
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