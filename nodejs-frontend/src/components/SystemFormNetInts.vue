<template>
  <v-container class="pa-0">
    <v-card outlined v-for="i in $root.netInts.length">
      <v-toolbar dense dark color="blue darken-1">
        <v-toolbar-title>Network Interface</v-toolbar-title>
        <v-spacer></v-spacer>
        <v-btn icon v-if="$root.netInts.length > 1">
          <v-icon v-on:click="removeNetInt(i - 1)">close</v-icon>
        </v-btn icon>
        <v-btn icon v-if="i == $root.netInts.length">
          <v-icon v-on:click="addNetInt">add_circle</v-icon>
        </v-btn icon>
      </v-toolbar>

      <v-container>
        <v-layout>
          <v-flex xs12 class="py-3">
            <v-text-field
              class="pa-0"
              v-model="$root.netInts[i - 1].id"
              label="Network Interface"
              :rules="[requiredRules]"
              validate-on-blur
            ></v-text-field>
          </v-flex>
        </v-layout>
      
        <SystemFormProtocols :netIntId="i - 1" />

      </v-container>
    </v-card>
  </v-container>
</template>

<script>
import { bus } from "../main";
import SystemFormProtocols from "./SystemFormProtocols";

export default {
  name: "SystemFormNetInts",
  components: {
    SystemFormProtocols
  },
  data() {
    return {
    }
  },
  methods: {
    addNetInt: function() {
      this.$root.netInts.push({
        id: null,
        protocols: [],
      });

      bus.$emit("formValidate");
    },
    removeNetInt: function(i) {
     this.$root.netInts.splice(i, 1);
    },
    requiredRules: value => !!value || "Required",
  },
  created: function() {
    if(this.$root.netInts.length == 0) this.addNetInt();
  },
  mounted: function() {
  }
};
</script>