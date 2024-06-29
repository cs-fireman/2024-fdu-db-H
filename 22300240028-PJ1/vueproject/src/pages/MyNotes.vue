<template>
    <div class="common-layout">
        <el-container>
            <el-header>
                <Header></Header>
            </el-header>
            <el-container>
                <el-aside width="200px">
                    <AsideBarNotes></AsideBarNotes>
                </el-aside>
                <el-main>
                    <div style="margin-bottom: 20px;">
                        <el-button type="primary" @click="saveNote">保存</el-button>
                        <el-button type="default" @click="increaseFontSize">增大字体</el-button>
                        <el-button type="default" @click="decreaseFontSize">减小字体</el-button>
                    </div>

                    <el-scrollbar height="800px">
                        <!-- <div>笔记 ID: {{ noteId }}</div> -->
                        <div class="editor">
                            <textarea class="input" :value="input" :style="{ fontSize: fontSize + 'px' }"
                                @input="update"></textarea>
                            <div class="output" v-html="output"></div>
                        </div>
                        <!-- <div>
                            <textarea v-model="formula" cols="30" rows="10" />
                            <vue-mathjax :formula="formula" />
                        </div> -->
                    </el-scrollbar>
                </el-main>
            </el-container>
        </el-container>
    </div>
</template>
<script setup>
import { marked } from 'marked'
import { debounce } from 'lodash-es'
import { ref, computed, defineProps, onMounted, onUnmounted } from 'vue'
import { useRoute } from 'vue-router';
import { useUserstore, useNotestore } from '@/store/user'
import Header from "@/components/Header.vue";
import AsideBarNotes from "@/components/AsideBarNotes.vue";
import { UpdateNoteContent } from "@/request/api";
const noteStore = useNotestore()
const input = ref('')
const output = computed(() => marked(input.value))
const update = debounce((e) => {
    input.value = e.target.value
}, 100)
const formula = ref('$$x = {-b \\pm \\sqrt{b^2-4ac} \\over 2a}.$$')
const fontSize = ref(14) // 默认字体大小
const increaseFontSize = () => fontSize.value += 1
const decreaseFontSize = () => fontSize.value = Math.max(12, fontSize.value - 1)

const route = useRoute();
const noteId = parseInt(route.params.note_id); // 使用路由参数
onMounted(() => {
    document.addEventListener('keydown', handleKeydown);
    // 确保组件挂载后设置input值
    const noteContent = noteStore.getNoteContentById(noteId);
    input.value = noteContent;
    // input.value = "Hello World!\n###hello###\n# hello\n"

});
// 组件卸载时移除事件监听
onUnmounted(() => {
    document.removeEventListener('keydown', handleKeydown);
});
// console.log('noteStore.notes:', noteStore.notes)
// const noteContent = noteStore.getNoteContentById(noteId);
// console.log('Note Content:', noteContent);
const handleKeydown = (event) => {
    // 检查是否按下了 Ctrl+S
    if (event.ctrlKey && event.key === 's') {
        event.preventDefault();  // 阻止默认的保存文件行为
        console.log('ctrl+s')  // 执行保存操作
        saveNote();
    }
};
const saveNote = () => {
    // 从文本编辑器中获取内容
    const noteContent = input.value;
    const updatedAt = new Date().toLocaleString('zh-CN', {
        timeZone: 'Asia/Shanghai', // 指定时区
        hour12: false // 24小时制
    });
    noteStore.updateNoteContent(noteId, noteContent, updatedAt);
    console.log('noteId:', noteId)
    console.log('noteStore.notes', noteStore.notes);
    updateData(noteId, noteContent, updatedAt);
};
const updateData = async (note_id, noteContent, updatedAt) => {
    // 在这里调用 API 获取数据，使用 currentPage 作为参数
    // const updatedAt = new Date().toLocaleString('zh-CN', {
    //     timeZone: 'Asia/Shanghai', // 指定时区
    //     hour12: false // 24小时制
    // });
    console.log('time', updatedAt);
    let res = await UpdateNoteContent({
        noteId: note_id,
        noteContent: noteContent,
        updateTime: updatedAt
    });
    console.log(res);

};
</script>
<style>
body {
    margin: 0;
}

.editor {
    height: 100vh;
    display: flex;
    border: 1px solid #ccc;
    border-radius: 10px;
}

.input,
.output {
    overflow: auto;
    width: 50%;
    height: 100%;
    box-sizing: border-box;
    padding: 0 20px;
    word-wrap: break-word;
    /* 打破长单词以确保换行 */
    white-space: pre-wrap;
    /* 保留空白，但允许正常换行 */
}

.output {
    padding-top: 20px;
    font-family: 'Consolas', 'Courier New', monospace;
}

.input {
    border: none;
    border-right: 1px solid #ccc;
    resize: none;
    outline: none;
    background-color: #f6f6f6;
    font-size: 14px;
    font-family: 'Monaco', courier, monospace;
    padding: 20px;
}

code {
    color: #f66;
}
</style>
