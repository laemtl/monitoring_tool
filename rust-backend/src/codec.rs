#![allow(dead_code)]
use bytes::{BufMut, BytesMut};
use serde::{Deserialize, Serialize};
use serde_json as json;
use std::io;
use tokio_io::codec::{Decoder, Encoder};

use crate::analysis::{Data,  MetricMsg};

#[derive(Serialize, Deserialize, Debug, Message)]
pub enum Msg {
    Init,
    Data
}

/// Codec for Client -> Server transport
pub struct SnifferCodec;

impl Decoder for SnifferCodec {
    type Item = MetricMsg;
    type Error = io::Error;

    fn decode(&mut self, src: &mut BytesMut) -> Result<Option<Self::Item>, Self::Error> {
        let mut len = src.split_to(1);
        if(len.len() == 0) {
            Ok(None)
        } else {
            let mut str = src.split_to(len[0] as usize);
            let metric: MetricMsg = protobuf::parse_from_bytes(&str).unwrap();
            Ok(Some(metric))
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
