<template>
  <div class="section small-padding">
    <div class="container">
      <div class="columns">
        <div class="form column is-one-third has-background-warning">
          <p class="error" v-if="error">{{ error }}</p>
          <h3 class="title is-5">Entities</h3>
          <div class="columns is-variable is-1" v-for="i in ip.length">
              <div class="column tiny-padding is-three-quarters">
                  <input v-model="ip[i]" class="input" type="text" placeholder="IP" />
              </div>
              <div class="column tiny-padding">
                  <input v-model="port[i]" class="input" type="text" placeholder="Port" />
              </div>
          </div>
          <div class="columns is-variable is-1">
            <div class="column tiny-padding is-three-quarters">
              <button v-on:click="addIps" class="button is-light">Add new</button>
            </div>
          </div>

          <h3 class="title is-5">Time</h3>
          <div class="columns">
              <div class="column">
                  <input class="input" type="text" placeholder="Interval" />
              </div>
              <div class="column">
                  <input class="input" type="text" placeholder="Duration" />
              </div>
          </div>
          <h3 class="title is-5">Statistics</h3>
          <div class="field" v-for="stat in statistics">
            <input
              :id="supertrim(stat.toLowerCase())"
              type="checkbox"
              :name="supertrim(stat.toLowerCase())"
              class="switch"
              checked="checked"
            />
            <label :for="supertrim(stat.toLowerCase())">{{ stat }}</label>
          </div>

          <div class="field is-grouped">
            <p class="control">
              <a class="button is-primary">
                Submit
              </a>
            </p>
            <p class="control">
              <a class="button is-light">
                Cancel
              </a>
            </p>
          </div>
        </div>
        <div id="result" class="column background-grey-light">
          <h3 class="title is-5">Results</h3>
          <table class="table">
            <tr>
              <th>Statistics</th>
              <th>Value</th>
            </tr>
            <tr v-for="stat in statistics">
              <td>{{ stat }}</td>
              <td>{{ propValue }}</td>
            </tr>
          </table>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
  export default {
    name: 'Dashboard',
    data() {
      return {
        statistics: [
          "Request service time",
          "Throughput",
          "Error rate",
          "Host load"
        ],
        ip: [] ,
        port: [],
        propValue: null,
        error: null
      }
    },
    methods: {
      connect() {
        this.ws = new WebSocket('ws://localhost:40510');
        var el = this;

        // event emmited when connected
        this.ws.onopen = function () {
          console.log('websocket is connected ...')

          // sending a send event to websocket server
          el.ws.send('connected')
        }

        // event emmited when receiving message 
        this.ws.onmessage = function (ev) {
          el.propValue = ev.data;
        }
      },
      addIps: function () {
        this.ip.push('');
        this.port.push('');
      },
      supertrim(value) {
        return value.replace(/\s/g, '');
      }
    },
    created: function() {
      this.ip.push('');
      this.port.push('');
    },
    mounted : function () {
      console.log('Ready');
      this.connect();
    }
  }
</script>