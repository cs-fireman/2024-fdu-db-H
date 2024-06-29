<script setup>
import AsideBarCalender from "@/components/AsideBarCalender.vue";
import Header from "@/components/Header.vue";
// import IndexMain from "@/components/IndexMain.vue";
import Profile from "@/components/Profile.vue";
import { ref, onMounted, onUnmounted } from "vue";
import * as THREE from "three";
import NET from "vanta/dist/vanta.net.min";
import GLOBE from "vanta/dist/vanta.globe.min";
const index = ref();
let vantaEffect;
onMounted(() => {
    vantaEffect = NET({
        el: index.value,
        THREE: THREE,
        mouseControls: true,
        touchControls: true,
        gyroControls: false,
        minHeight: 958.00,
        minWidth: 200.00,
        scale: 1.00,
        scaleMobile: 1.00,
        color: 0xe8b9c9,
        backgroundColor: 0xffffff,
    });
});

onUnmounted(() => {
    if (vantaEffect) {
        vantaEffect.destroy();
    }
});
</script>

<template>
    <div class="common-layout" ref="index">
        <el-container>
            <el-header>
                <Header>
                </Header>
            </el-header>
            <el-container>
                <el-aside width="200px">
                    <AsideBarCalender></AsideBarCalender>
                </el-aside>

                <el-container>
                    <el-main>
                        <router-view></router-view>
                        <el-calendar class="cal">
                            <template #date-cell="{ data }">
                                <p :class="data.isSelected ? 'is-selected' : ''">
                                    {{ data.day.split('-').slice(1).join('-') }}
                                    {{ data.isSelected ? '✔️' : '' }}
                                </p>
                            </template>
                        </el-calendar>

                    </el-main>
                </el-container>

            </el-container>

        </el-container>
    </div>
</template>
	
<style scoped></style>