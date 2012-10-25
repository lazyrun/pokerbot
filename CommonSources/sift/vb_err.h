/*
 *------------------------------------------------------------------------------
 * Copyright (C) 2000, Vladimir N. Bichigov.
 *------------------------------------------------------------------------------
 * vb_err.h
 *------------------------------------------------------------------------------
 *  This file contains error codes.
 *------------------------------------------------------------------------------
 */

#ifndef __VB_ERR_H
#define __VB_ERR_H

/*
 * Error groups.
 * -------------
 */

#define VB_OK      0   /* success */
#define VB_RETRY   1   /* retry now */
#define VB_LATER   2   /* retry sometimes in future */
#define VB_FAIL    3   /* fail: no need retry */

/*
 * Common error codes.
 * -------------------
 */

#define VB_OK       0   /* success exit */
#define VB_ARG      11  /* error arguments */
#define VB_OPEN     12  /* can't open file */
#define VB_READ     13  /* error file reading */
#define VB_WRITE    14  /* error file writing */
#define VB_MEM      15  /* not enough memory */
#define VB_RENAME   16  /* rename error */
#define VB_FDISK    17  /* filesystem full */
#define VB_LSEEK    18  /* lseek error */
#define VB_VER      19  /* wrong version */
#define VB_FORMAT   20  /* error file format */
#define VB_LOCK     21  /* error locking file */
#define VB_BADIN    22  /* �������� ������� ���������� */
#define VB_BADENV   23  /* ��������/�������� ����� (Environment) */
#define VB_BADINI   24  /* ��������/�������� INI-���� */
#define VB_BADDBH   25  /* ��������/�������� DBH-���� */
#define VB_BADSRCH  26  /* ������ ���� ������ */
#define VB_STOP     27  /* ��������� �� ������� STOP */

#endif /* __VB_ERR_H */





