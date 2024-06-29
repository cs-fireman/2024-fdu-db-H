<script setup lang="ts">
import {
    Document,
    Menu as IconMenu,
    Setting,
    Edit,
    Clock,
    Bell
} from '@element-plus/icons-vue'
import { useRouter } from 'vue-router'
const router = useRouter()

function change(key: string, keyPath: string[]) {
    console.log(key)
}
</script>

<template>
    <el-menu default-active="2" class="el-menu-vertical-demo" :router=true style="height: 890px;">
        <!-- <el-menu-item index="/index/">
            <el-icon>
                <icon-menu />
            </el-icon>
            <span>个人信息</span>
        </el-menu-item>
        <el-menu-item index="/index/checkUserInfo">
            <el-icon>
                <document />
            </el-icon>
            <span>查看用户信息</span>
        </el-menu-item> -->
        <!-- <el-menu-item index="/index/todolist">
            <el-icon>
                <bell />
            </el-icon>
            <span>备忘录</span>
        </el-menu-item> -->
        <el-menu-item index="/index/calender">
            <el-icon>
                <clock />
            </el-icon>
            <span>日历</span>
        </el-menu-item>
        <el-menu-item index="/index/myNotesList">
            <el-icon>
                <edit />
            </el-icon>
            <span>我的笔记</span>
        </el-menu-item>
    </el-menu>
</template>

<style scoped></style>