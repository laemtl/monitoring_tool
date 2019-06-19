<template>
  <v-app>
    <v-content class="blue-grey lighten-5">
      <v-container fluid grid-list-lg>
          <v-layout row wrap>
            <SystemForm v-bind:statistics="statistics" />
            <v-flex xs12 v-for="i in 1">
              <span> Analysis {{ status }} </span>
              <v-layout class="pa-2" row wrap>
                <SystemResultGraph v-bind:graph="{
                  name: statistics[id],
                  id: id,
                  netInt: netInt,
                  interval: interval
                }" v-for="id in statsid" />
              </v-layout>
            </v-flex>
          </v-layout>
      </v-container>
    </v-content>
  </v-app>
</template>

<script>
import SystemForm from './components/SystemForm'
import SystemResultGraph from './components/SystemResultGraph'
import { configBus } from './main'

require("./assets/main.scss");
var accurateInterval = require('accurate-interval');

export default {
  name: 'App',
  components: {
    SystemForm,
    SystemResultGraph
  },
  props: { 
    statistics: {
      type: Object,
      default: function () {
        var stats = {};
        stats["rst"] = { 
          label: "Request service time",
          status: 1
        };
        stats["tp"] = { 
          label: "Throughput",
          status: 1,
          from: { ip: null, port: null },
          to: { ip: null, port: null }
        };
        stats["erate"] = { 
          label: "Error rate",
          status: 1   
        };
        stats["hload"] = { 
          label: "Host load",
          status: 1,
          from: { ip: null, port: null } 
        };
        return stats;
      }
    }
  },
  data () {
    return {
      statsid: Object.keys(this.statistics),
      ws: null,
      status: 'not started',
      netInt: [],
      interval: -1
    }
  },
  methods: {
    openSocketListeners() {
      this.ws = new WebSocket('ws://localhost:3001/ws/');
      var el = this;

      // event emmited when connected
      this.ws.onopen = function () {
        console.log('websocket is connected ...')

        // sending a send event to websocket server
        //el.ws.send('connected')
      }
      
      // event emmited when receiving message 
      this.ws.onmessage = function (message) {
        console.log(message.data);

        /*var data = JSON.parse(message.data);

        for(var i = 0; i < el.statsid.length; i++) {
          configBus.$emit(el.statsid[i], {
            netInt: data.netInt, 
            data: parseFloat(data[el.statsid[i]])
          });
        }*/
      }
    },
    closeSocketListeners() {
      if(this.ws != null)
        this.ws.close();
    }
  },
  created() {
    // Using the server bus
    configBus.$on('configSelected', (statsid) => {
      this.statsid = statsid;
    });

    configBus.$on('run', (params) => {
      var el = this;
      this.netInt = params.netInt;
      this.interval = params.interval;
      this.status = 'is running';
      var duration = parseInt(params.duration, 10) * 1000;
      
      var timer = accurateInterval(function(scheduledTime) {        
        el.closeSocketListeners();
        el.status = 'is stopped';
        timer.clear();
      }, duration);
      
      this.openSocketListeners();
      this.ws.addEventListener('open', function (event) {
        //console.log("opened");
        el.ws.send(JSON.stringify({
          type: "Init",
          net_int: params.netInt,
          interval: parseInt(params.interval),
          duration: parseInt(params.duration)
        }));
      });
    });

    configBus.$on('stop', () => {
      console.log("stop");
      this.closeSocketListeners();
    });
  }
}
</script>
