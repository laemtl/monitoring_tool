#![allow(dead_code)]
use bytes::{BufMut, BytesMut};
use serde::{Deserialize, Serialize};
use serde_json as json;
use std::io;
use tokio_io::codec::{Decoder, Encoder};

use crate::analysis::Data;

#[derive(Serialize, Deserialize, Debug, Message)]
pub enum Msg {
    Init,
    Data
}

/// Codec for Client -> Server transport
pub struct SnifferCodec;

impl Decoder for SnifferCodec {
    type Item = Data;
    type Error = io::Error;

    fn decode(&mut self, src: &mut BytesMut) -> Result<Option<Self::Item>, Self::Error> {
        //let mut is = CodedInputStream::from_bytes(&mut src);
        //let mut data: Data = protobuf::parse_length_delimited_from(&mut is).unwrap();
        //src.split_to(12);
        //let s = src.split_to(27);
        if src.len() >= 27 {
            //println!("{}", src.len());
            let data: Data = protobuf::parse_length_delimited_from_bytes(&src).unwrap();
            src.split_to(src.len());
            Ok(Some(data))
        } else {
            Ok(None)
        }

        /*let size = {
            if src.len() < 2 {
                return Ok(None);
            }
            BigEndian::read_u16(src.as_ref()) as usize
        };

        if src.len() >= size + 2 {
            src.split_to(2);
            let buf = src.split_to(size);
            //Ok(Some(json::from_slice::<Msg>(&buf)?))
            Ok(Some(data))
        } else {
            Ok(None)
        }*/
    }
}

impl Encoder for SnifferCodec {
    type Item = Msg;
    type Error = io::Error;

    fn encode(
        &mut self,
        msg: Msg,
        dst: &mut BytesMut,
    ) -> Result<(), Self::Error> {
        let msg = json::to_string(&msg).unwrap();
        let msg_ref: &[u8] = msg.as_ref();

        dst.reserve(msg_ref.len() + 2);
        dst.put_u16_be(msg_ref.len() as u16);
        dst.put(msg_ref);

        Ok(())
    }
}
