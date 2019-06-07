//#[macro_use]
extern crate log;

mod analysis;
use crate::analysis::{Init, Data};
use protobuf::{
    CodedInputStream, CodedOutputStream, Message, ProtobufResult, RepeatedField
};


use futures::Stream;
use tokio_codec::FramedRead;
use tokio_io::io::WriteHalf;
use tokio_io::AsyncRead;


use std::str::FromStr;
use serde::{Deserialize, Serialize};
use std::time::{Duration, Instant};
use actix::prelude::*;
use actix_files as fs;
use actix_web::{
    middleware, web, App, Error, HttpRequest, HttpResponse, HttpServer
};
use actix_web_actors::ws;
use actix::Context;

use std::net::{self, TcpListener, TcpStream, IpAddr, Ipv4Addr, SocketAddr};
use std::io::{self, BufReader, BufWriter, Read };
use std::fs::File;
use std::{thread, time};

/// do websocket handshake and start `MyWebSocket` actor
fn ws_index(r: HttpRequest, stream: web::Payload) -> Result<HttpResponse, Error> {
    println!("{:?}", r);
    let res = ws::start(Ws, &r, stream);
    println!("{:?}", res.as_ref().unwrap());
    res
}

#[derive(Serialize, Deserialize, Debug)]
#[serde(tag = "type")]
enum Msg {
    Init { 
        net_int: Vec<String>,
        interval: u32,
        duration: u32
    }
}

struct Ws;

impl Actor for Ws {
    type Context = ws::WebsocketContext<Self>;
}

fn init (ctx: &mut ws::WebsocketContext<Ws>, net_int: Vec<String>, interval: u32, duration: u32) -> io::Result<()> {
    // TODO
    //let mut conn_req = ConnectionRequest::new();

    let mut init = Init::new();
    init.set_interval(interval);
    init.set_duration(duration);

    let net_int = RepeatedField::from_vec(net_int);
    init.set_netInt(net_int);
    
    let socket = SocketAddr::new(IpAddr::V4(Ipv4Addr::new(0, 0, 0, 0)), 3000);
    let mut stream = TcpStream::connect(&socket).unwrap();

    let mut os = CodedOutputStream::new(&mut stream);
    init.write_length_delimited_to(&mut os)?;
    os.flush()?;

    println!("{:?}", init);
    
    //while(true) { 
    let mut is = CodedInputStream::new(&mut stream);
    let mut data: Data = protobuf::parse_length_delimited_from(&mut is).unwrap();
    println!("{:?}", data);
    ctx.text("rs: ".to_string() + &data.get_rst().to_string());
    //}   

    Ok(())
}

/// Handler for `ws::Message`
impl StreamHandler<ws::Message, ws::ProtocolError> for Ws {
    fn started(&mut self, ctx: &mut Self::Context) {
        println!("WebSocket session openned");
    }

    fn finished(&mut self, ctx: &mut Self::Context) {
        println!("WebSocket session closed");
    }
    
    fn handle(&mut self, msg: ws::Message, ctx: &mut Self::Context) {
        // process websocket messages
        match msg {
            ws::Message::Ping(msg) => {
            }
            ws::Message::Pong(_) => {
            }
            ws::Message::Text(text) => {
                println!("WS: {:?}", text);
                
                let m: Msg = match serde_json::from_str(&text){
                    Result::Ok(val) => {val},
                    Result::Err(err) => {panic!("Unable to parse json: {}",err)}
                };

                match m {
                    Msg::Init { net_int, interval, duration } => init(ctx, net_int, interval, duration)
                }.unwrap();
            }

            ws::Message::Binary(bin) => {
            }
            ws::Message::Close(_) => {
                ctx.stop();
            }
            ws::Message::Nop => (),
        }
    }
}

/// Define tcp server that will accept incoming tcp connection and create
/// chat actors.
pub struct TcpServer;

impl TcpServer {
    pub fn new(_s: &str) {
        // Create server listener
        let addr = net::SocketAddr::from_str("0.0.0.0:3000").unwrap();
        let listener = TcpListener::bind(&addr).unwrap();

        // Our chat server `Server` is an actor, first we need to start it
        // and then add stream on incoming tcp connections to it.
        // TcpListener::incoming() returns stream of the (TcpStream, net::SocketAddr)
        // items So to be able to handle this events `Server` actor has to
        // implement stream handler `StreamHandler<(TcpStream,
        // net::SocketAddr), io::Error>`
        /*TcpServer::create(|ctx| {
            ctx.add_message_stream(
                listener.incoming()
                    /*.map_err(|_| ())*/
                    .map(|s| TcpConnect(s)),
            );
            TcpServer {}
        });*/
    }
}

/// Make actor from `Server`
impl Actor for TcpServer {
    /// Every actor has to provide execution `Context` in which it can run.
    type Context = Context<Self>;
}

#[derive(Message)]
struct TcpConnect(TcpStream);

/// Handle stream of TcpStream's
impl Handler<TcpConnect> for TcpServer {
    type Result = ();

    fn handle(&mut self, msg: TcpConnect, ctx: &mut Context<Self>) {
        // For each incoming connection we create `ChatSession` actor
        // with out chat server address.
        /*let server = self.chat.clone();
        ChatSession::create(|ctx| {
            let (r, w) = msg.0.split();
            ChatSession::add_stream(FramedRead::new(r, ChatCodec), ctx);
            ChatSession::new(server, actix::io::FramedWrite::new(w, ChatCodec, ctx))
        });*/

        /*let mut is = CodedInputStream::new(&mut stream);
        while(true) { 
            let mut data: Data = protobuf::parse_length_delimited_from(&mut is).unwrap();
            println!("{:?}", data);
            ctx.text(data.get_rst().to_string());
        }*/   
        //let ten_millis = time::Duration::from_millis(100);
        
        //while(true) {
            //ctx.text("myString");
            //thread::sleep(ten_millis);
        //}
    }
}

fn main() -> io::Result<()> {
    std::env::set_var("RUST_LOG", "actix_server=info,actix_web=info");
    env_logger::init();
    
    HttpServer::new(|| {
        App::new()
            // enable logger
            .wrap(middleware::Logger::default())
            // websocket route
            .service(web::resource("/ws/").route(web::get().to(ws_index)))
            // static files
            .service(fs::Files::new("/", "static/").index_file("index.html"))
    })
    
    // start http server on 127.0.0.1:3001
    .bind("127.0.0.1:3001")?
    .run()
}