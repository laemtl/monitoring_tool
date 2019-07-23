/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: analysis.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "analysis.pb-c.h"
void   analysis__init__init
                     (Analysis__Init         *message)
{
  static const Analysis__Init init_value = ANALYSIS__INIT__INIT;
  *message = init_value;
}
size_t analysis__init__get_packed_size
                     (const Analysis__Init *message)
{
  assert(message->base.descriptor == &analysis__init__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t analysis__init__pack
                     (const Analysis__Init *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &analysis__init__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t analysis__init__pack_to_buffer
                     (const Analysis__Init *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &analysis__init__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Analysis__Init *
       analysis__init__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Analysis__Init *)
     protobuf_c_message_unpack (&analysis__init__descriptor,
                                allocator, len, data);
}
void   analysis__init__free_unpacked
                     (Analysis__Init *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &analysis__init__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   analysis__data__init
                     (Analysis__Data         *message)
{
  static const Analysis__Data init_value = ANALYSIS__DATA__INIT;
  *message = init_value;
}
size_t analysis__data__get_packed_size
                     (const Analysis__Data *message)
{
  assert(message->base.descriptor == &analysis__data__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t analysis__data__pack
                     (const Analysis__Data *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &analysis__data__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t analysis__data__pack_to_buffer
                     (const Analysis__Data *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &analysis__data__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Analysis__Data *
       analysis__data__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Analysis__Data *)
     protobuf_c_message_unpack (&analysis__data__descriptor,
                                allocator, len, data);
}
void   analysis__data__free_unpacked
                     (Analysis__Data *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &analysis__data__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor analysis__init__field_descriptors[3] =
{
  {
    "interval",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_UINT32,
    0,   /* quantifier_offset */
    offsetof(Analysis__Init, interval),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "duration",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_UINT32,
    0,   /* quantifier_offset */
    offsetof(Analysis__Init, duration),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "netInt",
    3,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_STRING,
    offsetof(Analysis__Init, n_netint),
    offsetof(Analysis__Init, netint),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned analysis__init__field_indices_by_name[] = {
  1,   /* field[1] = duration */
  0,   /* field[0] = interval */
  2,   /* field[2] = netInt */
};
static const ProtobufCIntRange analysis__init__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor analysis__init__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "analysis.Init",
  "Init",
  "Analysis__Init",
  "analysis",
  sizeof(Analysis__Init),
  3,
  analysis__init__field_descriptors,
  analysis__init__field_indices_by_name,
  1,  analysis__init__number_ranges,
  (ProtobufCMessageInit) analysis__init__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor analysis__data__field_descriptors[21] =
{
  {
    "time",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT64,
    0,   /* quantifier_offset */
    offsetof(Analysis__Data, time),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "netInt",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Analysis__Data, netint),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "rstAvg",
    3,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_rstavg),
    offsetof(Analysis__Data, rstavg),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "rstMin",
    4,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_rstmin),
    offsetof(Analysis__Data, rstmin),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "rstMax",
    5,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_rstmax),
    offsetof(Analysis__Data, rstmax),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "rstClient",
    6,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_rstclient),
    offsetof(Analysis__Data, rstclient),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "tp",
    7,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_tp),
    offsetof(Analysis__Data, tp),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "errRate",
    8,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_errrate),
    offsetof(Analysis__Data, errrate),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "errRateMin",
    9,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_errratemin),
    offsetof(Analysis__Data, errratemin),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "errRateMax",
    10,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_errratemax),
    offsetof(Analysis__Data, errratemax),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "errRateClient",
    11,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_errrateclient),
    offsetof(Analysis__Data, errrateclient),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "reqRate",
    12,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_reqrate),
    offsetof(Analysis__Data, reqrate),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "reqRateMin",
    13,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_reqratemin),
    offsetof(Analysis__Data, reqratemin),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "reqRateMax",
    14,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_reqratemax),
    offsetof(Analysis__Data, reqratemax),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "reqRateClient",
    15,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_reqrateclient),
    offsetof(Analysis__Data, reqrateclient),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "clientAvg",
    16,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_clientavg),
    offsetof(Analysis__Data, clientavg),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "clientMin",
    17,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_clientmin),
    offsetof(Analysis__Data, clientmin),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "clientMax",
    18,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_clientmax),
    offsetof(Analysis__Data, clientmax),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "pathAvg",
    19,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_pathavg),
    offsetof(Analysis__Data, pathavg),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "pathMin",
    20,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_pathmin),
    offsetof(Analysis__Data, pathmin),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "pathMax",
    21,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_DOUBLE,
    offsetof(Analysis__Data, has_pathmax),
    offsetof(Analysis__Data, pathmax),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned analysis__data__field_indices_by_name[] = {
  15,   /* field[15] = clientAvg */
  17,   /* field[17] = clientMax */
  16,   /* field[16] = clientMin */
  7,   /* field[7] = errRate */
  10,   /* field[10] = errRateClient */
  9,   /* field[9] = errRateMax */
  8,   /* field[8] = errRateMin */
  1,   /* field[1] = netInt */
  18,   /* field[18] = pathAvg */
  20,   /* field[20] = pathMax */
  19,   /* field[19] = pathMin */
  11,   /* field[11] = reqRate */
  14,   /* field[14] = reqRateClient */
  13,   /* field[13] = reqRateMax */
  12,   /* field[12] = reqRateMin */
  2,   /* field[2] = rstAvg */
  5,   /* field[5] = rstClient */
  4,   /* field[4] = rstMax */
  3,   /* field[3] = rstMin */
  0,   /* field[0] = time */
  6,   /* field[6] = tp */
};
static const ProtobufCIntRange analysis__data__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 21 }
};
const ProtobufCMessageDescriptor analysis__data__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "analysis.Data",
  "Data",
  "Analysis__Data",
  "analysis",
  sizeof(Analysis__Data),
  21,
  analysis__data__field_descriptors,
  analysis__data__field_indices_by_name,
  1,  analysis__data__number_ranges,
  (ProtobufCMessageInit) analysis__data__init,
  NULL,NULL,NULL    /* reserved[123] */
};
