# Observability Schema

Logport and JetStream empower application developers and system administrators with modern observability. This is a turn-key solution for stable, performant, and scalable introspection into what your applications are doing, right now.

[Logport](https://github.com/homer6/logport) watches log files and sends changes to kafka (one line per message). Logport enables your applications to easily produce observability types (obtypes): Metrics, application Events, Telemetry, Traces, and Logs (METTL). Once in Kafka, [Jetstream](https://github.com/homer6/jetstream) can ship your obtypes to compatible "heads" (indices or dashboards) such as Elasticsearch, Logz.io, Snowflake, Lightstep, S3, or Prometheus.

Adding observability to your applications and servers is simple. There are no SDKs. No dependencies. No elaborate code to write. All you need is the ability to write to a file.

You do, however, need to follow the METTL schema, which is described in this document. 

Everything that your application outputs (to stdout/stderr or to file), should be single-line JSON that specifies an `obtype` (Observability Type).



## Logs

If you decline to provide an `obtype` key or value, or if you do not specify your output as JSON, it'll be interpreted as a log entry.

```
{ 
    "obtype": "log", 
    "arbitrary_value_1": "abc", 
    "arbitrary_value_2": [ "123", "456" ], 
    "arbitrary_value_3": { "ready": "player", "one": 1 } 
}
```


## Metrics

```
{ 
    "obtype": "metric",         //required
    "metric_type": "counter",   //required - see: https://prometheus.io/docs/concepts/metric_types/
    "name": "my_metric_name",   //required
    "value_int64": "55",        //exactly one "value_*" key is required
    "value_int32": "55",        //exactly one "value_*" key is required
    "value_uint64": "55",       //exactly one "value_*" key is required
    "value_uint32": "55",       //exactly one "value_*" key is required
    "value_float64": "55.1",    //exactly one "value_*" key is required
    "value_float32": "55.2",    //exactly one "value_*" key is required
    "labels": {
        "job": "my_app_name",   //required
        "instance": "server1.app_name.com",
        "label_1": "okay",
        "label_2": "hello"
    }
}
```


## Telemetry

```
{ 
    "obtype": "telemetric",     //required
    "arbitrary_value_1": "abc", 
    "arbitrary_value_2": [ "123", "456" ], 
    "arbitrary_value_3": { "ready": "player", "one": 1 }
}
```


## Traces

All times are UTC unix time (epoch seconds with nanoseconds as decimals).

```
{ 
    "obtype": "trace",
    "trace_id": "16 byte id",
    "span_id": "8 byte id",
    "parent_span_id": "8 byte id",
    "span_kind": "unspecified|server|client",
    "name": "span_name",
    "start_time": "1559930259.863529482",
    "end_time": "1559930259.863529483",
    "annotations": [
        {
            "time": "1559930259.863529483",
            "message": "my message",
            "attributes": { 
                "attribute_1": "hello", 
                "attribute_2": 32,
                "attribute_3": false
            }
        },
        {
            "time": "1559930259.863529484",
            "message": "my message 2",
            "attributes": { 
                "attribute_1": "hello", 
                "attribute_2": 32,
                "attribute_3": false
            }
        }
    ],
    "message_events": [
        {
            "time": "1559930259.863529485",
            "message_event_type": "unspecified|sent|received",
            "message_id": 565656,
            "uncompressed_byte_size": 45,
            "compressed_byte_size": 12          
        },
        {
            "time": "1559930259.863529486",
            "message_event_type": "unspecified|sent|received",
            "message_id": 232323,
            "uncompressed_byte_size": 48,
            "compressed_byte_size": 15          
        }
    ],
    "attributes": { 
        "attribute_1": "player", 
        "attribute_2": 55.25,
        "attribute_3": true
    },
    "links": [
        {
            "trace_id": 12378978456,
            "span_id": 564648,
            "link_type": "unspecified|child|parent",
            "attributes": { 
                "attribute_1": "red", 
                "attribute_2": 7,
                "attribute_3": false
            }
        }
    ],
    "status":{
        "status_code": 0, //success
        "message": "Event description."
    }
}
```


## (Application) Events

```
{ 
    "obtype": "event", 
    "name": "request_received", 
    "arbitrary_value_1": "abc", 
    "arbitrary_value_2": [ "123", "456" ], 
    "arbitrary_value_3": { "ready": "player", "one": 1 }
}
```



## (Application) Exception

```
{ 
    "obtype": "exception", 
    "component": "worker", 
    "name": "exception_processing_request", 
    "description": "e.what()"
}
```


## Error

```
{ 
    "obtype": "error", 
    "component": "worker", 
    "name": "error_processing_request", 
    "description": "Failed to parse request."
}
```

