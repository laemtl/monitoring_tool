//#[macro_use]
extern crate log;

//use protobuf::{CodedInputStream, CodedOutputStream, Message, ProtobufResult, RepeatedField};
mod analysis;
use crate::analysis::Init;

use protobuf::{CodedInputStream, CodedOutputStream, Message, RepeatedField};
//use protobuf::error::{ProtobufResult, ProtobufError};

use serde::{Deserialize, Serialize};
use std::time::{Duration, Instant};

use actix::prelude::*;
use actix_files as fs;
use actix_web::{
    middleware, web, App, Error, HttpRequest, HttpResponse, HttpServer
};
use actix_web_actors::ws;
//use futures::Future;

use std::net::TcpStream;
//use byteorder::{ByteOrder, LittleEndian};


//use bytes::BytesMut;
//use std::io::prelude::*;
//use std::io;
use std::net::{IpAddr, Ipv4Addr, SocketAddr};






//use byteorder::{ByteOrder, BigEndian};
//use retry::retry;
//use bincode;
use std::io::{
    self,
    //BufReader,
    //BufWriter,
    //Read,
};


/// How often heartbeat pings are sent
const HEARTBEAT_INTERVAL: Duration = Duration::from_secs(5);
/// How long before lack of client response causes a timeout
const CLIENT_TIMEOUT: Duration = Duration::from_secs(10);


/// A connection to an sccache server.
/*pub struct ServerConnection {
    /// A reader for the socket connected to the server.
    reader : BufReader<TcpStream>,
    /// A writer for the socket connected to the server.
    writer : BufWriter<TcpStream>,
}*/

/*impl ServerConnection {
    /// Create a new connection using `stream`.
    pub fn new(stream : TcpStream) -> io::Result<ServerConnection> {
        let writer = stream.try_clone()?;
        Ok(ServerConnection {
            reader : BufReader::new(stream),
            writer : BufWriter::new(writer),
        })
    }

    /// Send `request` to the server, read and return a `Response`.
    fn request(&mut self, request: Request) -> Result<Response> {
        trace!("ServerConnection::request");
        //util::write_length_prefixed_bincode(&mut self.writer, request)?;
        trace!("ServerConnection::request: sent request");
        //self.read_one_response()
    }

    /// Read a single `Response` from the server.
    pub fn read_one_response(&mut self) -> Result<Response> {
        trace!("ServerConnection::read_one_response");
        let mut bytes = [0; 4];
        self.reader.read_exact(&mut bytes).chain_err(|| "Failed to read response header")?;
        let len = BigEndian::read_u32(&bytes);
        trace!("Should read {} more bytes", len);
        let mut data = vec![0; len as usize];
        self.reader.read_exact(&mut data)?;
        trace!("Done reading");
        Ok(bincode::deserialize(&data)?)
    }
}*/

/// do websocket handshake and start `MyWebSocket` actor
fn ws_index(r: HttpRequest, stream: web::Payload) -> Result<HttpResponse, Error> {
    println!("{:?}", r);
    let res = ws::start(MyWebSocket::new(), &r, stream);
    println!("{:?}", res.as_ref().unwrap());
    res
}

/// websocket connection is long running connection, it easier
/// to handle with an actor
struct MyWebSocket {
    /// Client must send ping at least once per 10 seconds (CLIENT_TIMEOUT),
    /// otherwise we drop connection.
    hb: Instant,
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

impl Actor for MyWebSocket {
    type Context = ws::WebsocketContext<Self>;
    
    /// Method is called on actor start. We start the heartbeat process here.
    fn started(&mut self, ctx: &mut Self::Context) {
        self.hb(ctx);
    }
}

/*fn write_with_len_prefix(data: impl AsRef<[u8]>, mut writer: impl io::Write) -> io::Result<()> {
    let data = data.as_ref();
    let mut buf = [0u8; 4];
    LittleEndian::write_u32(&mut buf, data.len() as u32);
    writer.write_all(&buf)?;
    writer.write_all(data)?;
    Ok(())
}*/

/*fn write_frame(io: T) {
    let mut transport = length_delimited::Framed::new(io);
    let frame = BytesMut::from("hello world");
    transport.send(frame).wait().unwrap();
}*/

/// Establish a TCP connection to an sccache server listening on `port`.
/*fn connect_to_server(port : u16) /*-> io::Result<ServerConnection>*/ {
    trace!("connect_to_server({})", port);
    let stream = (TcpStream::connect(("0.0.0.0", port))) /*?*/;
    //ServerConnection::new(stream)
}*/


/// Attempt to establish a TCP connection to an sccache server listening on `port`.
///
/// If the connection fails, retry a few times.
/*pub fn connect_with_retry(port: u16) -> io::Result<ServerConnection> {
    trace!("connect_with_retry({})", port);
    // TODOs:
    // * Pass the server Child in here, so we can stop retrying
    //   if the process exited.
    // * Send a pipe handle to the server process so it can notify
    //   us once it starts the server instead of us polling.
    match retry(10, 500, || connect_to_server(port), |res| res.is_ok()) {
        Ok(Ok(conn)) => Ok(conn),
        _ => Err(io::Error::new(io::ErrorKind::TimedOut, "Connection to server timed out")),
    }
}*/

fn init (net_int: Vec<String>, interval: u32, duration: u32) -> io::Result<()> {
    let mut init = Init::new();
    init.set_interval(interval);
    init.set_interval(duration);

    let net_int = RepeatedField::from_vec(net_int);
    init.set_netInt(net_int);
    
    //println!("we got an init");
    let socket = SocketAddr::new(IpAddr::V4(Ipv4Addr::new(0, 0, 0, 0)), 3000);
    let mut stream = TcpStream::connect(&socket).unwrap();
    //connect_to_server(3000);

    let mut os = CodedOutputStream::new(&mut stream);
    init.write_length_delimited_to(&mut os)?;

    println!("{:?}", init);

    Ok(())
}

/// Handler for `ws::Message`
impl StreamHandler<ws::Message, ws::ProtocolError> for MyWebSocket {
    fn handle(&mut self, msg: ws::Message, ctx: &mut Self::Context) {
        // process websocket messages
        println!("WS: {:?}", msg);
        match msg {
            ws::Message::Ping(msg) => {
                self.hb = Instant::now();
                ctx.pong(&msg);
            }
            ws::Message::Pong(_) => {
                self.hb = Instant::now();
            }
            ws::Message::Text(text) => {
                let m: Msg = match serde_json::from_str(&text){
                    Result::Ok(val) => {val},
                    Result::Err(err) => {panic!("Unable to parse json: {}",err)}
                };

                match m {
                    Msg::Init { net_int, interval, duration } => init(net_int, interval, duration)
                }.unwrap();
                
                ctx.text(text)
            }
            ws::Message::Binary(bin) => ctx.binary(bin),
            ws::Message::Close(_) => {
                ctx.stop();
            }
            ws::Message::Nop => (),
        }
    }
}

impl MyWebSocket {
    fn new() -> Self {
        Self { hb: Instant::now() }
    }

    /// helper method that sends ping to client every second.
    ///
    /// also this method checks heartbeats from client
    fn hb(&self, ctx: &mut <Self as Actor>::Context) {
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
    
    // start http server on 127.0.0.1:8080
    .bind("127.0.0.1:3001")?
    .run()
}