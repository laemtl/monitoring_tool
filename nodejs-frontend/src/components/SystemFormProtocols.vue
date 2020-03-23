<template>
  <v-container class="pa-0">
    <v-container v-for="i in protocols.length" class="pa-0">
      <v-card class="mx-auto" outlined>
        <v-toolbar dense dark color="blue-grey darken-1">
            <v-toolbar-title>Protocol</v-toolbar-title>
            <v-spacer></v-spacer>
            <v-btn icon v-if="protocols.length > 1">
                <v-icon v-on:click="removeProtocol(i - 1)">close</v-icon>
            </v-btn icon>
            <v-btn icon v-if="i == protocols.length">
                <v-icon v-on:click="addProtocol()">add_circle</v-icon>
            </v-btn icon>
        </v-toolbar>

        <v-container>
            <v-layout>
                <v-flex class="py-0">
                    <v-select v-model="protocols[i - 1].id" @input="updateProtocol($event, i - 1)" :items="$options.availProtocols" label="Protocol"></v-select>
                </v-flex>
            </v-layout>

            <v-layout>
              <v-flex md8 class="py-0">
                <v-text-field
                  class="pa-0 ma-0"
                  ref="clientIP"
                  v-model="protocols[i - 1].client.ip"
                  :rules="[ipRules, tpRules(i - 1, protocols[i - 1].client.ip)]"
                  validate-on-blur
                  label="Client IP"
                ></v-text-field>
              </v-flex>

              <v-flex md4 class="py-0">
                <v-text-field
                  class="pa-0 ma-0"
                  ref="clientPort"
                  v-model="protocols[i - 1].client.ports"
                  :rules="[portRules, tpRules(i - 1, protocols[i - 1].client.ports)]"
                  validate-on-blur
                  label="Client ports"
                ></v-text-field>
              </v-flex>
            </v-layout>

            <v-layout>
              <v-flex md8 class="pt-0">
                <v-text-field
                  class="pa-0 ma-0"
                  ref="serverIP"
                  v-model="protocols[i - 1].server.ip"
                  :rules="[ipRules, tpRules(i - 1, protocols[i - 1].server.ip)]"
                  validate-on-blur
                  label="Server IP"
                ></v-text-field>
              </v-flex>
              <v-flex md4 class="pt-0">
                <v-text-field
                  class="pa-0 ma-0"
                  ref="serverPort"
                  v-model="protocols[i - 1].server.ports"
                  :rules="[portRules, tpRules(i - 1, protocols[i - 1].server.ports)]"
                  validate-on-blur
                  label="Server ports"
                ></v-text-field>
              </v-flex>
            </v-layout>

            <v-layout row wrap>
              <v-flex md6 v-for="(metric, id) in protocols[i - 1].metrics">
                <v-layout row wrap align-center>
                  <v-flex md10>
                    <v-switch
                      class="ma-0 pa-0"
                      color="info"
                      v-model="metric.active"
                      :label="`${metric.label}`"
                      @change="metricChange()"
                    ></v-switch>
                    </v-flex>
                  </v-layout>             
                </v-flex>
            </v-layout>
        </v-container>
      </v-card>
    </v-container>
  </v-container>
</template>

<script>
import * as isIp from "is-ip";
import { bus } from "../main";
import protocolsData from '../assets/json/protocols.json';

const ipLength = 45;

export default {
  name: "SystemFormProtocols",
  props: ["netIntId"],
  data() {
    return {
      protocols: this.$root.netInts[this.netIntId].protocols,  
    }
  },
  availProtocols: Object.keys(protocolsData.protocols),
  methods: {
    addProtocol() {
      this.protocols.push({
        id: null,
        metrics: {},
        client: {
          ip: null,
          ports: null
        },
        server: {
          ip: null,
          ports: null
        }
      });
    },
    updateProtocol(protocol, i) {
      this.$set(this.protocols[i], 'id', protocol);
      
      this.$set(this.protocols[i], 'metrics', {});
      if(protocolsData.protocols[protocol] && protocolsData.protocols[protocol].metrics) {
        protocolsData.protocols[protocol].metrics.forEach(metric => {
          let m = {};
          Object.assign(m, protocolsData.metrics[metric]);
          this.$set(this.protocols[i].metrics, metric, m);
        });
      }
    },
    removeProtocol(i) {
      this.protocols.splice(i, 1);
    },
    metricChange() {
      bus.$emit("formValidate");
    },
    requiredRules: value => !!value || "Required",
    ipRules: value => { 
      return (isIp(value) && value.length <= ipLength) || value === null || value.length === 0 || "Invalid value"
    },
    portRules: value => {
      return (!!value && /^[0-9, ]*$/.test(value)) || value === null || value.length === 0 || "Invalid value"
    },
    tpRules(i, value) { 
      
      let result = !this.protocols[i] 
      || !this.protocols[i].metrics 
      || ((!this.protocols[i].metrics.tp || !this.protocols[i].metrics.tp.active) 
      && (!this.protocols[i].metrics.tp_rev || !this.protocols[i].metrics.tp_rev.active)) 
      || value !== null && value.length !== 0 
      || "Required when throuput is on";

      return result;
    }
  },
  created: function() {
    if(this.protocols.length == 0) this.addProtocol();
  },
  mounted: function() {
    
  }
};
</script>