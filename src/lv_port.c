/*
 * @Author       : Zeepunt.H
 * @Date         : 2022-03-12 14:08:44
 * @LastEditTime : 2022-05-28 02:06:59
 * Gitee : https://gitee.com/zeepunt
 * Github: https://github.com/Recaa
 *  
 * Copyright (c) 2022 by Zeepunt.H, All Rights Reserved. 
 */

#include <common.h>

static lv_disp_t *disp = NULL;
static lv_disp_drv_t disp_drv;
static lv_disp_draw_buf_t disp_buf;
static lv_color_t *buf = NULL;

static lv_indev_drv_t indev_drv;
static lv_indev_t *mouse_indev;

static lv_fs_drv_t fs_drv;
static char *fs_root_path = NULL;

void lv_port_disp_init(void)
{
    /* SDL2 初始化 */
    sdl_init();

    /* 分配显示缓冲区 */
    int buf_size = sizeof(lv_color_t) * SDL_HOR_RES * SDL_VER_RES;
    buf = malloc(buf_size);
    if (NULL != buf)
        lv_disp_draw_buf_init(&disp_buf, buf, NULL, buf_size);

    /* 默认初始化 */
    lv_disp_drv_init(&disp_drv);
    
    /* 缓冲区 */
    disp_drv.draw_buf = &disp_buf;
    /* 回调函数，需开发者提供，用于将缓冲区的内容复制到显示器的特定区域 */
    disp_drv.flush_cb = sdl_display_flush;
    /* 显示器的水平分辨率，单位是像素 */
    disp_drv.hor_res = SDL_HOR_RES;
    /* 显示器的垂直分辨率，单位是像素 */
    disp_drv.ver_res = SDL_VER_RES;

    /* 注册显示设备 */
    disp = lv_disp_drv_register(&disp_drv);
}

void lv_port_disp_deinit(void)
{
    if (NULL != buf)
    {
        free(buf);
        buf = NULL;
    }
}

void lv_port_indev_init(void)
{
    /* 默认初始 */
    lv_indev_drv_init(&indev_drv);
    
    /* 设置输入设备的类型 */
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    /* 从驱动程序获取输入事件，比如，坐标、事件、字符 */
    indev_drv.read_cb = sdl_mouse_read;

    /* 注册输入设备 */
    mouse_indev = lv_indev_drv_register(&indev_drv);

    /* 这里是设置一个光标，显示当前的位置（下面的代码可以忽略） */
    LV_IMG_DECLARE(mouse_cursor_icon);
    lv_obj_t * cursor_obj = NULL;
    cursor_obj = lv_img_create(lv_scr_act());
    lv_img_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(mouse_indev, cursor_obj);      // 设置光标
}

static int fs_init(void)
{
    int ret       = -1;
    char path[64] = {0};

    char *home = NULL;
    DIR  *root = NULL;

    home = getenv("PWD");
    sprintf(path, "%s/lvgl_fs", home);

    root = opendir(path);

    if (NULL == root)
    {
        LOG_DEBUG("try mkdir.");
        ret = mkdir(path, 0777);
        if (ret < 0)
        {
            LOG_ERROR("mkdir %s fail: %s.", path, strerror(errno));
            goto exit;
        }
    }
    else
    {
        closedir(root);
        fs_root_path = strdup(path);
    }

    ret = 0;
exit:
    return ret;
}

static bool fs_ready(struct _lv_fs_drv_t *drv)
{
    return true;
}

static void* fs_open(struct _lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    int fd = -1;
    uint32_t flags = 0;
    char buf[256] = {0};
    char *ptr = NULL;

    if (mode == LV_FS_MODE_WR)
        flags = O_WRONLY;
    else if (mode == LV_FS_MODE_RD)
        flags = O_RDONLY;
    else if (mode == (LV_FS_MODE_WR | LV_FS_MODE_RD))
        flags = O_RDWR;

    sprintf(buf, "%s%s", fs_root_path, path);
    LOG_DEBUG("open path: %s.", buf);

    fd = open(buf, flags);
    if (fd < 0) 
        return NULL;
    
    return (void *)(lv_uintptr_t)fd;
}

static lv_fs_res_t fs_close(struct _lv_fs_drv_t *drv, void *file_p)
{
    close((lv_uintptr_t)file_p);
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_read(struct _lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    *br = read((lv_uintptr_t)file_p, buf, btr);
    return (int32_t)(*br) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

static lv_fs_res_t fs_write(struct _lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw)
{
    *bw = write((lv_uintptr_t)file_p, buf, btw);
    return (int32_t)(*bw) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

static lv_fs_res_t fs_seek(struct _lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence)
{
    lseek((lv_uintptr_t)file_p, pos, whence);
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_tell(struct _lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    *pos_p = lseek((lv_uintptr_t)file_p, 0, SEEK_CUR);
    return LV_FS_RES_OK;
}

static void* fs_dir_open(struct _lv_fs_drv_t *drv, const char *path)
{
    char buf[256] = {0};

    sprintf(buf, "%s%s", fs_root_path, path);
    return opendir(buf);
}

static lv_fs_res_t fs_dir_read(struct _lv_fs_drv_t *drv, void *dir_p, char *fn)
{
    lv_fs_res_t res = LV_FS_RES_OK;
    struct dirent *entry;
    do 
    {
        entry = readdir(dir_p);
        if (NULL != entry) 
        {
            if (DT_DIR == entry->d_type)
                sprintf(fn, "/%s", entry->d_name);
            else
                strcpy(fn, entry->d_name);
        }
        else
        {
            strcpy(fn, "");
            res = LV_FS_RES_NOT_EX;
            break;
        }
    } while((strcmp(fn, "/.") == 0) || (strcmp(fn, "/..") == 0));
    
    return res;
}

static lv_fs_res_t fs_dir_close(struct _lv_fs_drv_t *drv, void *dir_p)
{
    closedir(dir_p);
    return LV_FS_RES_OK;
}

void lv_port_fs_init(void)
{
    int ret = -1;

    ret = fs_init();
    if (ret < 0)
    {
        LOG_ERROR("filesystem init fail.");
        return;
    }

    /* 默认初始化 */
    lv_fs_drv_init(&fs_drv);

    fs_drv.letter   = 'S';                 // 表示驱动器的大写字母，比如 C 盘
    fs_drv.ready_cb = fs_ready;            // 确认驱动已就绪
    fs_drv.open_cb  = fs_open;             // 打开文件
    fs_drv.close_cb = fs_close;            // 关闭文件
    fs_drv.read_cb  = fs_read;             // 读文件
    fs_drv.write_cb = fs_write;            // 写文件
    fs_drv.seek_cb  = fs_seek;             // 移动当前文件的光标
    fs_drv.tell_cb  = fs_tell;             // 获得当前文件光标位置

    fs_drv.dir_open_cb  = fs_dir_open;     // 打开目录
    fs_drv.dir_read_cb  = fs_dir_read;     // 读取目录的内容
    fs_drv.dir_close_cb = fs_dir_close;    // 关闭目录

    fs_drv.user_data = NULL;               // 用户数据

    /* 注册 fs 驱动 */
    lv_fs_drv_register(&fs_drv);
}