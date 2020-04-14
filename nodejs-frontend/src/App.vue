<template>
  <v-app>
    <v-content class="blue-grey lighten-5">
      <v-container fluid grid-list-lg>
        <v-layout row wrap>
          <SystemForm />
          <v-flex xs12>
              <span> Analysis {{ status }} </span>
              <v-tabs
                background-color="cyan"
                dark
                show-arrows
                v-model="tab"
              >
                <v-tabs-slider color="yellow"></v-tabs-slider>
                <div v-for="i in $root.netInts.length">   
                  <v-tab
                    v-for="j in $root.netInts[i - 1].protocols.length"
                    :key="i + j"
                  >
                    {{$root.netInts[i - 1].id }} - {{$root.netInts[i - 1].protocols[j - 1].id }}
                  </v-tab>
                </div>
              </v-tabs>
              
              <v-tabs-items v-model="tab">
                <div v-for="i in $root.netInts.length">   
                  <v-tab-item
                    v-for="j in $root.netInts[i - 1].protocols.length"
                    :key="i + j"
                  >
                    <v-layout class="pa-2" row wrap>
                      <SystemResultGraph 
                        v-for="(metric, id) in $root.netInts[i - 1].protocols[j - 1].metrics"
                        :data="{
                          netIntIndex: i - 1,
                          protocolIndex: j - 1,
                          metricId: id
                        }"
                      />
                    </v-layout>
                  </v-tab-item>
                </div>
              </v-tabs-items>            
          </v-flex>
        </v-layout>
      </v-container>
    </v-content>
  </v-app>
</template>

<script>
import SystemForm from "./components/SystemForm";
import SystemResultGraph from "./components/SystemResultGraph";
import { bus } from "./main";

require("./assets/main.scss");

export default {
  name: "App",
  components: {
    SystemForm,
    SystemResultGraph
  },
  data() {
    return {
      tab: 0,
      ws: null,
      status: "not started",
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
      if(typeof process.env.VUE_APP_WS_URL !== 'string') return;

      this.ws = new WebSocket(process.env.VUE_APP_WS_URL);
      var el = this;

      // event emmited when connected
      this.ws.onopen = function() {
        console.log("websocket is connected ...");
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
          console.log(data.net_int_id + '_' + data.protocol_id + '_' + data.name);
          if(typeof data.metric_avg !== "undefined") {
            bus.$emit(data.net_int_id + '_' + data.protocol_id + '_' + data.name, {
              data: data.metric_avg
            });
          } else if(typeof data.metric_cum_distr !== "undefined") {
            bus.$emit(data.net_int_id + '_' + data.protocol_id + '_' + data.name, {
              data: data.metric_cum_distr
            });
          }
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
    bus.$on("run", () => {
      console.log("sending");

      var el = this;
      this.status = "is running";
      this.openSocketListeners();

      let netInts = [];
      this.ws.addEventListener("open", function() {
        for (let n of el.$root.netInts) {
          let netInt = {
            id: n.id,
            protocols: []
          };
          
          for (let p of n.protocols) {
            let protocol = {
              id: p.id,
              activeMetrics: 0,
              
              clientIp: el.ip2int(p.client.ip),
              clientPorts: ((p.client.ports != null) ? p.client.ports.split(",").map(i => parseInt(i)) : []),
              
              serverIp: el.ip2int(p.server.ip),
              serverPorts: ((p.server.ports != null) ? p.server.ports.split(",").map(i => parseInt(i)) : []),
            };

            let i = 0;
            for (let m in p.metrics) {
              if (p.metrics[m].active) {
                protocol.activeMetrics |= 1 << i;
              }
              i++;
            }

            netInt.protocols.push(protocol);
          }

          netInts.push(netInt);
        }

        let msg = {
          type: "Init",
          net_ints: netInts,
          interval: parseInt(el.$root.interval),
          duration: parseInt(el.$root.duration)
        };

        //console.log(msg);

        el.ws.send(
          JSON.stringify(msg)
        );
      });
    });

    bus.$on("stop", () => {
      console.log("stop");
      this.closeSocketListeners();
    });
  }
};
</script>