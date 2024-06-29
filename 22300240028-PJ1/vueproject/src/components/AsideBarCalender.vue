<script setup lang="ts">
import {
    Document,
    Menu as IconMenu,
    Setting,
    Edit,
    ArrowLeft
} from '@element-plus/icons-vue'
import { useRouter } from 'vue-router'
const router = useRouter()




function change(key: string, keyPath: string[]) {
    console.log(key)
}

function goToLastPage() {
    router.push('/index')
}
</script>

<template>
    <el-menu default-active="1" class="el-menu-vertical-demo" :router=true style="height: 890px;">
        <el-menu-item @click="goToLastPage">
            <el-icon>
                <arrow-left />
            </el-icon>
            <span>返回</span>
        </el-menu-item>
    </el-menu>
</template>

<style scoped></style>