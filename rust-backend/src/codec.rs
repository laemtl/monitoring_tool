#![allow(dead_code)]
use bytes::{BufMut, BytesMut};
use serde::{Deserialize, Serialize};
use serde_json as json;
use std::io;
use tokio_io::codec::{Decoder, Encoder};

use crate::analysis::{MetricMsg};

#[derive(Serialize, Deserialize, Debug, Message)]
pub enum Msg {
    Init,
    Data,
    NetInt    
}

/// Codec for Client -> Server transport
pub struct SnifferCodec;

impl Decoder for SnifferCodec {
    type Item = MetricMsg;
    type Error = io::Error;

    fn decode(&mut self, src: &mut BytesMut) -> Result<Option<Self::Item>, Self::Error> {
        if src.len() == 0 {
            //println!("len 0");
            Ok(None)
        } else {
            let len = src[0] as usize;
            if src.len() < len + 1 {
                //println!("not enough len");
                Ok(None)
            } else {
                src.split_to(1);
                let str = src.split_to(len);
                let metric: MetricMsg = protobuf::parse_from_bytes(&str)?;
                //println!("len: {}", len);
                Ok(Some(metric))
            }
        }
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