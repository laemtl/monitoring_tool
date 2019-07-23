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
                  stat: stat,
                  id: id,
                  netInt: netInt,
                  interval: interval
                }" v-for="(stat, id) in activeStats" />
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
          active: true,
          max: 0.01
        };
        stats["reqRate"] = { 
          label: "Request rate",
          active: true,
          max: 4,
          from: { ip: null, port: null } 
        };
        stats["errRate"] = { 
          label: "Error rate",
          active: true,
          max: 1000
        };
        stats["tp"] = { 
          label: "Throughput",
          active: true,
          max: 5,
          from: { ip: null, port: null },
          to: { ip: null, port: null }
        };
        stats["client"] = { 
          label: "Clients",
          active: true,
          max: 10
        };
        stats["path"] = { 
          label: "Paths",
          active: true,
          max: 10
        };
        return stats;
      }
    }
  },
  data() {
    return {
      ws: null,
      status: 'not started',
      netInt: [],
      interval: -1
    }
  },
  computed: {
    activeStats() {
      var activeStats = {};
      for (var id in this.statistics) {
        if(this.statistics[id].active) {
          activeStats[id] = this.statistics[id];
        }
      }
      return activeStats;
    }
  },
  methods: {
    openSocketListeners() {
      //this.ws = new WebSocket('ws://bmj-cluster.cs.mcgill.ca:15480/ws/');
      this.ws = new WebSocket('ws://127.0.0.1:8081/ws/');
      var el = this;

      // event emmited when connected
      this.ws.onopen = function () {
        console.log('websocket is connected ...')

        // sending a send event to websocket server
        //el.ws.send('connected')
      }
 
      // event emmited when connected
      this.ws.onclose = function () {
        console.log('websocket is closed ...')
        el.status = 'is stopped';
      }
      
      // event emmited when receiving message 
      this.ws.onmessage = function (message) {
        try {
          var data = JSON.parse(message.data);
          
          for (var id in el.activeStats) {
            if(typeof data[id] !== 'undefined') {
              configBus.$emit(id, {
                netInt: data.netInt, 
                data: data[id]
              });
            }
          }
        } catch(e) {
            alert(e); // error in the above string (in this case, yes)!
        }
      }
    },
    closeSocketListeners() {
      if(this.ws != null)
        this.ws.close();
    }
  },
  created() {
    // Using the server bus
    configBus.$on('configSelected', (id, status) => {
      this.statistics[id].active = status;
    });

    configBus.$on('run', (params) => {
      var el = this;
      this.netInt = params.netInt;
      this.interval = params.interval;
      this.status = 'is running';
      var duration = parseInt(params.duration, 10) * 1000;
      
      /*var timer = accurateInterval(function(scheduledTime) {        
        el.closeSocketListeners();
        el.status = 'is stopped';
        timer.clear();
      }, duration);*/
      
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
