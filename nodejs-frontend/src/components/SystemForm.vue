<template>
  <v-container fluid class="pa-0">
    <v-navigation-drawer v-model="drawer" absolute temporary width="470">
      <v-flex xs12 pa-0>
        <v-toolbar dense dark color="primary">
          <v-toolbar-title>System analysis</v-toolbar-title>
        </v-toolbar>

        <v-form class="pa-3" ref="systemForm" v-on:submit="submit">
          <v-container class="px-0">
            <p class="error" v-if="error">{{ error }}</p>

            <v-card-text class="text-xs-center">
              <v-btn color="primary" class="my-0 ml-0" v-on:click="submit"
                >Run</v-btn
              >
              <v-btn outline color="primary" class="my-0" v-on:click="stop"
                >Stop</v-btn
              >
            </v-card-text>

            <v-layout v-for="i in netInt.length">
              <v-flex xs12 md11 class="py-0">
                <v-text-field
                  class="pa-0"
                  v-model="netInt[i - 1]"
                  label="Network Interface (name)"
                  :rules="[rules.requiredRules]"
                  validate-on-blur
                ></v-text-field>
              </v-flex>
              <v-icon v-if="netInt.length > 1" v-on:click="removeNetInt(i - 1)"
                >close</v-icon
              >
              <v-icon v-if="i == netInt.length" v-on:click="addNetInt"
                >add_circle</v-icon
              >
            </v-layout>
            <v-layout>
              <v-flex md6 class="py-0">
                <v-text-field
                  class="pa-0 ma-0"
                  v-model="interval"
                  :rules="[rules.requiredRules, rules.timeRules]"
                  validate-on-blur
                  label="Interval (s)"
                ></v-text-field>
              </v-flex>
              <v-flex md6 class="py-0">
                <v-text-field
                  class="pa-0 ma-0"
                  v-model="duration"
                  :rules="[rules.requiredRules, rules.timeRules]"
                  validate-on-blur
                  label="Duration (s)"
                ></v-text-field>
              </v-flex>
            </v-layout>

            <v-layout>
              <v-flex md8 class="py-0">
                <v-text-field
                  class="pa-0 ma-0"
                  ref="clientIP"
                  v-model="client.ip"
                  :rules="[rules.ipRules, rules.tpRules]"
                  validate-on-blur
                  label="Client IP"
                ></v-text-field>
              </v-flex>

              <v-flex md4 class="py-0">
                <v-text-field
                  class="pa-0 ma-0"
                  ref="clientPort"
                  v-model="client.port"
                  :rules="[rules.portRules, rules.tpRules]"
                  validate-on-blur
                  label="Client port"
                ></v-text-field>
              </v-flex>
            </v-layout>

            <v-layout>
              <v-flex md8 class="pt-0">
                <v-text-field
                  class="pa-0 ma-0"
                  ref="serverIP"
                  v-model="server.ip"
                  :rules="[rules.ipRules, rules.tpRules]"
                  validate-on-blur
                  label="Server IP"
                ></v-text-field>
              </v-flex>
              <v-flex md4 class="pt-0">
                <v-text-field
                  class="pa-0 ma-0"
                  ref="serverPort"
                  v-model="server.port"
                  :rules="[rules.portRules, rules.tpRules]"
                  validate-on-blur
                  label="Server port"
                ></v-text-field>
              </v-flex>
            </v-layout>

            <v-layout row wrap>
              <v-flex md6 
                v-for="(stat, id) in statistics" 
                v-if="id != 'tp_rev'">
                <v-layout row wrap align-center>
                  <v-flex md10>
                    <v-switch
                      class="ma-0 pa-0"
                      color="info"
                      v-model="stat.active"
                      :label="`${stat.label}`"
                      v-on:change="configChange(id)"
                    ></v-switch>
                  </v-flex>
                  <v-flex md2 
                    class="swap-btn"
                    v-if="id == 'tp'">
                    <v-btn fab small dark
                      class="pa-0 ma-0"
                      color="primary" 
                      v-model="statistics.tp_rev.active"
                      :class="{disabled: !statistics.tp_rev.active}"
                      @click="statistics.tp_rev.active = !statistics.tp_rev.active">
                      <v-icon>swap_horiz</v-icon>
                    </v-btn> 
                  </v-flex>
                </v-layout>             
              </v-flex>
            </v-layout>
          </v-container>
        </v-form>
      </v-flex>
    </v-navigation-drawer>
    <v-btn color="pink" dark @click.stop="drawer = !drawer">Open Form</v-btn>
  </v-container>
</template>

<script>
const isIp = require("is-ip");
const ipLength = 45,
  portLength = 5;
import { configBus } from "../main";

export default {
  name: "SystemForm",
  props: ["statistics"],
  data() {
    return {
      drawer: null,
      netInt: [],
      status: true,
      interval: null,
      duration: null,
      client: {
        ip: null,
        port: null
      },
      server: {
        ip: null,
        port: null
      },
      ipLength: ipLength,
      portLength: portLength,
      rules: {
        requiredRules: value => !!value || "Required",
        timeRules: value => (!isNaN(value) && value > 0) || "Invalid value",
        tpRules: value => { 
          return !this.statistics.tp.active || value !== null && value.length !== 0 || "Required when throuput is on"
        },
        ipRules: value => { 
          return (isIp(value) && value.length <= ipLength) || value === null || value.length === 0 || "Invalid value"
        },
        portRules: value => {
          return (!!value && !isNaN(value) && value.length <= portLength) || value === null || value.length === 0 || "Invalid value"
        },
      },
      error: null
    };
  },
  methods: {
    addNetInt: function() {
      this.netInt.push("");
    },
    removeNetInt: function(i) {
      this.netInt.splice(i, 1);
    },
    supertrim(value) {
      return value.replace(/\s/g, "");
    },
    submit() {
      if (this.$refs.systemForm.validate()) {
        configBus.$emit("run", {
          interval: this.interval,
          duration: this.duration,
          netInt: this.netInt,
          client: this.client,
          server: this.server
        });
        this.drawer = false;
      }
    },
    stop() {
      configBus.$emit("stop");
    },
    configChange: function(id) {
      // Using the config bus
      configBus.$emit("configSelected", id, this.statistics[id].active);
    }
  },
  created: function() {
    this.addNetInt();
  },
  mounted: function() {
    //console.log('Ready');
  }
};
</script>

<style>
.v-navigation-drawer__border {
  display: none;
}

.v-input .v-input__slot {
  margin-bottom: 0 !important;
}

.v-btn--floating.v-btn--small {
  height: 25px;
  width: 25px;
}

.swap-btn {
  margin-top: -10px !important;
}

.swap-btn button.disabled {
  background: rgba(0, 0, 0, 0.12) !important;
}

.swap-btn button.disabled i {
  color: #000 !important;
}
</style>