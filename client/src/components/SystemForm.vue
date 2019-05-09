<template>
    <v-flex xs12 sm6 md4>
        <v-card>
            <v-toolbar dense dark color="primary">
                <v-toolbar-title>System analysis</v-toolbar-title>
            </v-toolbar>
            <v-card-text>
                <p class="error" v-if="error">{{ error }}</p>
                <v-form ref="systemForm" v-on:submit="submit">
                    <v-container class="px-0">
                        <v-layout v-for="(item, i) in netInt">
                            <v-flex xs12 md11>
                                <v-text-field class="pa-0"
                                    v-model="item.name"
                                    label="Network Interface (name)"
                                    required
                                ></v-text-field>
                            </v-flex>
                            <!--<v-flex xs12 md9>
                                <v-text-field class="pa-0"
                                    v-model="item.ip"
                                    :rules="ipRules"
                                    validate-on-blur
                                    :counter=ipLength
                                    label="IP"
                                    required
                                ></v-text-field>
                            </v-flex>
                            <v-flex xs12 md3>
                                <v-text-field class="pa-0"
                                    v-model="item.port"
                                    :rules="portRules"
                                    validate-on-blur
                                    :counter=portLength
                                    label="Port"
                                    required
                                ></v-text-field>
                            </v-flex>-->
                            <v-icon v-if="i > 0 && (i != netInt.length - 1 || netInt.length < 3)" v-on:click="removeNetInt(i)">close</v-icon>
                            <v-icon v-if="i == netInt.length - 1" v-on:click="addNetInt">add_circle</v-icon>
                        </v-layout>
                        
                        <!--<v-layout>
                            <v-btn v-on:click="addNetInt" color="primary">Add</v-btn>
                        </v-layout>-->
                        
                        <v-layout>
                            <v-flex md6>
                                <v-text-field class="pa-0 ma-0"
                                v-model="interval"
                                :rules="timeRules"
                                validate-on-blur    
                                label="Interval (s)"
                                required
                                ></v-text-field>
                            </v-flex>
                            <v-flex md6>
                                <v-text-field class="pa-0 ma-0"
                                v-model="duration"
                                :rules="timeRules"
                                validate-on-blur 
                                label="Duration (s)"
                                ></v-text-field>
                            </v-flex>
                        </v-layout>
                        <v-switch class="ma-0" color="info" 
                            v-model="stats" 
                            v-for="stat in statistics" 
                            :name="supertrim(stat.toLowerCase())" 
                            :label="`${ stat }`" 
                            :value="`${ stat }`"
                            v-on:change="configChange"
                        ></v-switch>
                    </v-container>
                    <v-btn color="primary" class="my-0 ml-0" v-on:click="submit">Run</v-btn>
                    <v-btn outline color="primary" class="my-0" v-on:click="stop">Stop</v-btn>
                </v-form>
            </v-card-text>
        </v-card>
    </v-flex>
</template>

<script>    
const isIp = require('is-ip');
const ipLength = 45, portLength = 5;
import { configBus } from '../main';

export default {
    name: 'SystemForm',
    props: ['statistics'],
    data() {
      return {
        netInt: [], // network interfaces
        stats: this.statistics, 
        interval: null,
        duration: null,
        ipLength: ipLength,
        portLength: portLength,
        ipRules: [v => !!v && isIp(v) && v.length <= ipLength || 'Invalid IP'],
        portRules: [v => !!v && !isNaN(v) && v.length > 0 && v.length <= portLength || 'Invalid port'],
        timeRules: [v => !!v && !isNaN(v) && v.length > 0 && v > 0 || 'Invalid value'],
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
        addNetInt: function () {
            //this.netInt.push({ip:'', port:''});
            this.netInt.push('');
        },
        removeNetInt: function (i) {
            this.netInt.splice(i, 1);
        },
        supertrim(value) {
            return value.replace(/\s/g, '');
        },
        submit () {
            this.$refs.systemForm.validate();
            console.log("toto");
        },
        stop () { 
        },
        configChange: function () {
            // Using the config bus
            configBus.$emit('configSelected', this.stats);
        }
    },
    created: function() {
      this.addNetInt();
    },
    mounted : function () {
      console.log('Ready');
      this.connect();
    }
  }
</script>