<template>
    <v-container fluid class="pa-0">
        <v-navigation-drawer
        v-model="drawer"
        absolute
        temporary
        width=400
        >
            <v-flex xs12 pa-0>
                
                    <v-toolbar dense dark color="primary">
                        <v-toolbar-title>System analysis</v-toolbar-title>
                    </v-toolbar>
                    
                        <v-form class="pa-3" ref="systemForm" v-on:submit="submit">
                            <v-container class="px-0">
                                <p class="error" v-if="error">{{ error }}</p>                        
                                
                                <v-card-text class="text-xs-center">
                                    <v-btn color="primary" class="my-0 ml-0" v-on:click="submit">Run</v-btn>
                                    <v-btn outline color="primary" class="my-0" v-on:click="stop">Stop</v-btn>
                                </v-card-text>
                                
                                <v-layout v-for="i in netInt.length">
                                    <v-flex xs12 md11>
                                        <v-text-field class="pa-0"
                                            v-model="netInt[i-1]"
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
                                    <v-icon v-if="netInt.length > 1" v-on:click="removeNetInt(i-1)">close</v-icon>
                                    <v-icon v-if="i == netInt.length" v-on:click="addNetInt">add_circle</v-icon>
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
                                    v-model="statsid" 
                                    v-for="(stat, id) in statistics" 
                                    :name="`${ id }`" 
                                    :label="`${ stat }`" 
                                    :value="`${ id }`"
                                    v-on:change="configChange"
                                ></v-switch>

                                <!--<v-flex xs12 md9>
                                    <v-text-field class="pa-0"
                                        v-model="ip"
                                        :rules="ipRules"
                                        validate-on-blur
                                        :counter=ipLength
                                        label="IP"
                                        required
                                    ></v-text-field>
                                </v-flex>
                                <v-flex xs12 md3>
                                    <v-text-field class="pa-0"
                                        v-model="port"
                                        :rules="portRules"
                                        validate-on-blur
                                        :counter=portLength
                                        label="Port"
                                        required
                                    ></v-text-field>
                                </v-flex>-->
                            </v-container>
                        </v-form>
            </v-flex>
        </v-navigation-drawer>
        <v-btn
            color="pink"
            dark
            @click.stop="drawer = !drawer"
        >Open Form</v-btn>
    </v-container>
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
        drawer: null,
        netInt: [],
        statsid:  Object.keys(this.statistics), 

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
            if(this.$refs.systemForm.validate()) {
                configBus.$emit('run', { 
                    interval: this.interval, 
                    duration: this.duration,
                    netInt: this.netInt
                });
                this.drawer = false;
            }
        },
        stop () { 
            configBus.$emit('stop');
        },
        configChange: function () {
            // Using the config bus
            configBus.$emit('configSelected', this.statsid);
        }
    },
    created: function() {
      this.addNetInt();
    },
    mounted : function () {
      console.log('Ready');
    }
  }
</script>

<style>
.v-navigation-drawer__border {
    display: none;
}
</style>