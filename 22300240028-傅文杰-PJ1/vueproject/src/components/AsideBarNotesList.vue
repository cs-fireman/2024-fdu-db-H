<script setup lang="ts">
import {
    Document,
    Menu as IconMenu,
    Setting,
    Edit,
    ArrowLeft
} from '@element-plus/icons-vue'
import { useRouter } from 'vue-router'
import { reactive, ref } from 'vue'
import { useNotestore, useUserstore } from '@/store/user'
import { AddNote } from '@/request/api'
import { useRoute } from 'vue-router';
const router = useRouter()
const noteStore = useNotestore()
const userStore = useUserstore()
const dialogVisible = ref(false)
const route = useRoute();

const formLabelWidth = '140px'

const form = reactive({
    name: '',
    region: '',
    date1: '',
    date2: '',
    delivery: false,
    type: [],
    resource: '',
    desc: '',
})
const handleCreateNote = async () => {
    console.log("处理创建笔记逻辑")
    dialogVisible.value = false;
    const maxId = noteStore.getMaxNoteId();
    const updatedAt = new Date().toLocaleString('zh-CN', {
        timeZone: 'Asia/Shanghai', // 指定时区
        hour12: false // 24小时制
    });
    const username = userStore.userName;
    const userId = userStore.userId;
    // console.log('maxId:', maxId);
    // noteStore.addNote({
    //     note_id: 0,
    //     user_id: userId,
    //     note_content: "",
    //     note_title: form.name,
    //     update_time: updatedAt,
    //     is_active: true,
    //     user_name: username
    // })
    // console.log('noteStore.notes:', noteStore.notes)
    const res = await addData(maxId + 1, userId, form.name, updatedAt, username);
    console.log('res note id:', res.noteId.note_id);
    if (res.success) {
        noteStore.addNote({
            note_id: res.noteId.note_id,
            user_id: userId,
            note_content: "",
            note_title: form.name,
            update_time: updatedAt,
            is_active: true,
            user_name: username
        })
        console.log('noteStore.notes:', noteStore.notes)
    }
}
function change(key: string, keyPath: string[]) {
    console.log(key)
}
const addData = async (noteId: number, userId: number, noteTitle: string, updateTime: string, userName: string) => {
    // 在这里调用 API 获取数据，使用 currentPage 作为参数
    let res = await AddNote({
        noteId: noteId,
        userId: userId,
        noteTitle: noteTitle,
        updateTime: updateTime,
        userName: userName
    });
    console.log('addData', res);
    return res;

};
function goToLastPage() {
    router.push('/index')
}
</script>

<template>
    <el-menu default-active="1" class="el-menu-vertical-demo" :router=true style="height: 890px;">
        <el-menu-item @click="dialogVisible = true">
            <el-icon>
                <edit />
            </el-icon>
            <span>新建笔记</span>
        </el-menu-item>
        <el-menu-item @click="goToLastPage">
            <el-icon>
                <arrow-left />
            </el-icon>
            <span>返回</span>
        </el-menu-item>
    </el-menu>

    <el-dialog v-model="dialogVisible" title="新建笔记" width="500">
        <el-form :model="form">
            <el-form-item label="标题" :label-width="formLabelWidth">
                <el-input v-model="form.name" autocomplete="off" />
            </el-form-item>
            <!-- <el-form-item label="Zones" :label-width="formLabelWidth">
                <el-select v-model="form.region" placeholder="Please select a zone">
                    <el-option label="Zone No.1" value="shanghai" />
                    <el-option label="Zone No.2" value="beijing" />
                </el-select>
            </el-form-item> -->
        </el-form>
        <template #footer>
            <div class="dialog-footer">
                <el-button @click="dialogVisible = false">取消</el-button>
                <el-button type="primary" @click="handleCreateNote">
                    确认
                </el-button>
            </div>
        </template>
    </el-dialog>
</template>

<style scoped></style>