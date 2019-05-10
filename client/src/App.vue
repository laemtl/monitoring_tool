<template>
  <v-app>
    <v-content class="blue-grey lighten-5">
      <v-container fluid grid-list-lg>
          <v-layout row wrap>
            <SystemForm v-bind:statistics="statistics" />
            <v-flex xs12 v-for="i in 1">
              <span> Analysis {{ status }} </span>
              <v-layout class="pa-2" row wrap>
                <SystemResultGraph v-bind:name="stat" v-for="stat in stats" />
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
      type: Array,
      default: function () {
        return [
          "Request service time",
          "Throughput",
          "Error rate",
          "Host load"
        ]
      }
    }
  },
  data () {
    return {
      stats: this.statistics,
      ws: null,
      status: 'not started'
    }
  },
  methods: {
    openSocketListeners() {
      this.ws = new WebSocket('ws://localhost:40510');
      var el = this;

      // event emmited when connected
      this.ws.onopen = function () {
        console.log('websocket is connected ...')

        // sending a send event to websocket server
        el.ws.send('connected')
      }
      
      // event emmited when receiving message 
      this.ws.onmessage = function (message) {
        configBus.$emit('message', message.data);
      }
    },
    closeSocketListeners() {
      if(this.ws != null)
        this.ws.close();
    }
  },
  created() {
    // Using the server bus
    configBus.$on('configSelected', (stats) => {
      this.stats = stats;
    });

    configBus.$on('run', (params) => {
      var el = this;
      this.status = 'is running';
      var duration = parseInt(params.duration, 10) * 1000;
      
      var timer = accurateInterval(function(scheduledTime) {        
        el.closeSocketListeners();
        el.status = 'is stopped';
        timer.clear();
      }, duration);
      
      this.openSocketListeners();
    });

    configBus.$on('stop', () => {
      this.closeSocketListeners();
    });
  }
}
</script>