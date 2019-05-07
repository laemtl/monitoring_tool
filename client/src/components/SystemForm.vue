<template>
    <v-flex xs12 sm6 md4>
        <v-card>
            <v-toolbar dense dark color="primary">
                <v-toolbar-title>System analysis</v-toolbar-title>
            </v-toolbar>
            <v-card-text>
                <p class="error" v-if="error">{{ error }}</p>
                <v-form ref="systemForm" v-on:submit="submit">
                    <v-container class="pa-0">
                        <v-layout v-for="(a, i) in addr">
                            <v-flex xs12 md9>
                                <v-text-field class="pa-0"
                                    v-model="a.ip"
                                    :rules="ipRules"
                                    validate-on-blur
                                    :counter=ipLength
                                    label="IP"
                                    required
                                ></v-text-field>
                            </v-flex>
                            <v-flex xs12 md3>
                                <v-text-field class="pa-0"
                                    v-model="a.port"
                                    :rules="portRules"
                                    validate-on-blur
                                    :counter=portLength
                                    label="Port"
                                    required
                                ></v-text-field>
                            </v-flex>
                            <v-icon v-if="i > 0" v-on:click="removeAddr(i)">close</v-icon>
                        </v-layout>
                        
                        <v-layout>
                            <v-btn v-on:click="addAddr" color="primary">Add</v-btn>
                        </v-layout>
                        
                        <v-layout>
                            <v-flex md6>
                                <v-text-field
                                v-model="interval"
                                :rules="timeRules"
                                validate-on-blur    
                                label="Interval"
                                required
                                ></v-text-field>
                            </v-flex>
                            <v-flex md6>
                                <v-text-field
                                v-model="duration"
                                :rules="timeRules"
                                validate-on-blur 
                                label="Duration"
                                ></v-text-field>
                            </v-flex>
                        </v-layout>
                        <v-switch class="ma-0" v-for="stat in statistics" :name="supertrim(stat.toLowerCase())" :label="`${ stat }`"></v-switch>
                    </v-container>
                    <v-btn color="primary" class="ma-0" v-on:click="submit">Submit</v-btn>
                </v-form>
            </v-card-text>
        </v-card>
    </v-flex>
</template>

<script>
  const isIp = require('is-ip');
  const ipLength = 45, portLength = 5;

  export default {
    name: 'SystemForm',
    props: ['statistics'],
    data() {
      return {
        addr: [],
        interval: null,
        duration: null,
        ipLength: ipLength,
        portLength: portLength,
        ipRules: [v => !!v && isIp(v) && v.length <= ipLength || 'Invalid IP'],
        portRules: [v => !!v && !isNaN(v) && v.length > 0 && v.length <= portLength || 'Invalid port'],
        timeRules: [v => !!v && !isNaN(v) && v.length > 0 || 'Invalid value'],
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
      addAddr: function () {
        this.addr.push({ip:'', port:''});
      },
      removeAddr: function (i) {
        this.addr.splice(i, 1);
      },
      supertrim(value) {
        return value.replace(/\s/g, '');
      },
      submit () {
        console.log("submit");
        this.$refs.systemForm.validate() 
      }
    },
    created: function() {
      this.addAddr();
    },
    mounted : function () {
      console.log('Ready');
      this.connect();
    }
  }
</script>