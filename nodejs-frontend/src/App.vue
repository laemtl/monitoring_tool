<template>
  <v-app>
    <v-content class="blue-grey lighten-5">
      <v-container fluid grid-list-lg>
        <v-layout row wrap>
          <SystemForm v-bind:statistics="statistics" />
          <v-flex xs12>
            <span> Analysis {{ status }} </span>
            <v-layout class="pa-2" row wrap>
              <SystemResultGraph
                v-for="(stat, id) in statistics"
                v-if="id != 'tp_rev'"            
                v-bind:graph="{
                  stat: stat,
                  id: id,
                  netInt: netInt,
                  interval: interval
                }"
              />
            </v-layout>
          </v-flex>
        </v-layout>
      </v-container>
    </v-content>
  </v-app>
</template>

<script>
import SystemForm from "./components/SystemForm";
import SystemResultGraph from "./components/SystemResultGraph";
import { configBus } from "./main";

require("./assets/main.scss");
//var accurateInterval = require('accurate-interval');

//console.log(process.env);

export default {
  name: "App",
  components: {
    SystemForm,
    SystemResultGraph
  },
  props: {
    statistics: {
      type: Object,
      default: function() {
        var stats = {};
        stats["rst"] = {
          label: "Request service time",
          type: "line",
          active: true,
          max: 0.5
        };
        stats["req_rate"] = {
          label: "Request rate",
          type: "line",
          active: true,
          max: 10
        };
        stats["err_rate"] = {
          label: "Error rate",
          type: "line",
          active: true,
          max: 1.1
        };
        stats["tp"] = {
          label: "Throughput",
          type: "line",
          active: true,
          max: 1000
        };
        stats["tp_rev"] = {
          active: true,
        };
        stats["conn_rate"] = {
          label: "Connection rate",
          type: "line",
          active: true,
          max: 2
        };
        stats["clients"] = {
          label: "Clients",
          type: "area",
          active: true,
          max: 1.1
        };
        stats["req_path"] = {
          label: "Request paths",
          type: "area",
          active: true,
          max: 1.1
        };
        stats["req_method"] = {
          label: "Request methods",
          type: "area",
          active: true,
          max: 1.1
        };
        stats["req_type"] = {
          label: "Request type",
          type: "area",
          active: true,
          max: 1.1
        };
        stats["rsp_status"] = {
          label: "Response status",
          type: "area",
          active: true,
          max: 1.1
        };

        return stats;
      }
    }
  },
  data() {
    return {
      ws: null,
      status: "not started",
      netInt: [],
      interval: -1,
      client: {
        ip: null,
        port: null
      },
      server: {
        ip: null,
        port: null
      }
    };
  },
  methods: {
    ip2int(ip) {
      if (!ip) return null;
      return (
        ip.split(".").reduce(function(ipInt, octet) {
          return (ipInt << 8) + parseInt(octet, 10);
        }, 0) >>> 0
      );
    },
    openSocketListeners() {
      console.log(process.env.VUE_APP_WS_URL);
      if(typeof process.env.VUE_APP_WS_URL !== 'string') return;

      this.ws = new WebSocket(process.env.VUE_APP_WS_URL);
      var el = this;

      // event emmited when connected
      this.ws.onopen = function() {
        console.log("websocket is connected ...");

        // sending a send event to websocket server
        //el.ws.send('connected')
      };

      // event emmited when connected
      this.ws.onclose = function() {
        console.log("websocket is closed ...");
        el.status = "is stopped";
      };

      // event emmited when receiving message
      this.ws.onmessage = function(message) {
        try {
          var data = JSON.parse(message.data);
          
          console.log(data);

          if("tp" in data && "tp_rev" in data) {
            data.tp.rev_avg = data.tp_rev.avg;
            data.tp.rev_min = data.tp_rev.min;
            data.tp.rev_max = data.tp_rev.max;

            delete data.tp_rev;
          }

          //console.log(data);

          //for (var id in data) {
            //console.log(id);
            //console.log(data[id]);
          

            if(typeof el.statistics[data.name] !== 'undefined' && typeof data.metricAvg !== "undefined") {
              configBus.$emit(data.name, {
                netInt: data.netInt,
                data: data.metricAvg
              });
            } else if(typeof el.statistics[data.name] !== 'undefined' && typeof data.metricCumDistr !== "undefined") {
              configBus.$emit(data.name, {
                netInt: data.netInt,
                data: data.metricCumDistr
              });
            }
          //}
        } catch (e) {
          alert(e); // error in the above string (in this case, yes)!
        }
      };
    },
    closeSocketListeners() {
      if (this.ws != null) this.ws.close();
    }
  },
  created() {
    // Using the server bus
    configBus.$on("configSelected", (id, status) => {
      this.statistics[id].active = status;
    });

    configBus.$on("run", params => {
      var el = this;
      this.netInt = params.netInt;
      this.interval = params.interval;
      this.status = "is running";
      //var duration = parseInt(params.duration, 10) * 1000;

      /*var timer = accurateInterval(function(scheduledTime) {        
        el.closeSocketListeners();
        el.status = 'is stopped';
        timer.clear();
      }, duration);*/

      this.openSocketListeners();

      this.ws.addEventListener("open", function() {
        //console.log("opened");

        let active_metric = 0;
        let stats = Object.keys(el.statistics);

        for (let id in el.statistics) {
          if (el.statistics[id].active) {
            let index = stats.indexOf(id);
            active_metric |= 1 << index;
          }
        }

        //console.log(params);

        el.ws.send(
          JSON.stringify({
            type: "Init",
            net_int: params.netInt,
            interval: parseInt(params.interval),
            duration: parseInt(params.duration),
            top_client_cnt: parseInt(params.clientCnt),
            active_metric: active_metric,
            client_ip: el.ip2int(params.client.ip),
            client_port: parseInt(params.client.port),
            server_ip: el.ip2int(params.server.ip),
            server_port: parseInt(params.server.port)
          })
        );
      });
    });

    configBus.$on("stop", () => {
      console.log("stop");
      this.closeSocketListeners();
    });
  }
};
</script>
