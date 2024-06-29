<script setup lang="ts">
import { ElMessage, ElNotification as notify } from 'element-plus'
import { LogoutApi } from "@/request/api";
import { useRouter } from 'vue-router'
import { ref } from 'vue'

const router = useRouter()
const activeIndex = ref('1')
const handleSelect = (key: string, keyPath: string[]) => {
    console.log(key, keyPath)
}
function gotoRoot() {
    router.push('/')
}
function gotoIndex() {
    router.push('/index')
}
async function logout() {
    let res = await LogoutApi()
    if (res.success) {
        ElMessage.success("登出成功")
        await router.push('/')
    } else {
        ElMessage("登出失败")
    }
    console.log(res)
}
</script>

<template>
    <!-- <div style="display: flex; justify-content: space-between; align-items: center; ">
        <img src="https://img2.imgtp.com/2024/04/05/DMHKG7pg.jpg" alt="Logo" style="height: 50px;">
        <div>
            <el-button type="info" @click="logout">登出</el-button>
        </div>
    </div> -->
    <el-menu :default-active="activeIndex" class="el-menu-demo" mode="horizontal" :ellipsis="false" @select="handleSelect">
        <el-menu-item index="0">
            <img style="width: 60px" src="https://img2.imgtp.com/2024/04/05/DMHKG7pg.jpg" alt="Element logo" />
        </el-menu-item>
        <div class="flex-grow" />
        <el-menu-item index="1"> </el-menu-item>
        <el-menu-item index="4" @click="gotoRoot">回到首页</el-menu-item>
        <el-menu-item index="2" @click="gotoIndex">回到主页</el-menu-item>
        <el-menu-item index="3" @click="logout">退出登录</el-menu-item>
    </el-menu>
</template>

<style scoped>
.flex-grow {
    flex-grow: 1;
}

/* 隐藏菜单选中指示器 */
.el-menu-item.is-active {
    border-bottom: none !important;
    /* 移除下划线 */
}
</style>