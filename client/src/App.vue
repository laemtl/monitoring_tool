<template>
  <v-app>
    <v-content class="blue-grey lighten-5">
      <v-container fluid grid-list-lg>
          <v-layout row wrap>
            <SystemForm v-bind:statistics="statistics" />
            <v-flex xs12 sm6 md8>
              <v-layout row wrap>
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
      stats: this.statistics
    }
  },
  created() {
    // Using the server bus
    configBus.$on('configSelected', (stats) => {
      this.stats = stats;
    });
  }
}
</script>