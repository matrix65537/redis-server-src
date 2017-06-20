/* Select()-based ae.c module
 * Copyright (C) 2009-2010 Salvatore Sanfilippo - antirez@gmail.com
 * Released under the BSD license. See the COPYING file for more info. */

#include <string.h>

//˽�нṹ�壬����ά���ڲ�״̬
typedef struct aeApiState {
    fd_set rfds, wfds;
    /* We need to have a copy of the fd sets as it's not safe to reuse
     * FD sets after select(). */
    fd_set _rfds, _wfds;
} aeApiState;

//Ϊ�ڲ�״̬����ռ䣬�����г�ʼ�������ڲ�״ָ̬�뱣�浽�ϲ㴫�ݵĽṹ֮��
static int aeApiCreate(aeEventLoop *eventLoop) {
    aeApiState *state = zmalloc(sizeof(aeApiState));

    if (!state) return -1;
    FD_ZERO(&state->rfds);
    FD_ZERO(&state->wfds);
    eventLoop->apidata = state;
    return 0;
}

static void aeApiFree(aeEventLoop *eventLoop) {
    zfree(eventLoop->apidata);
}

//����ض���fd��Ӷ���д�¼�
static int aeApiAddEvent(aeEventLoop *eventLoop, int fd, int mask) {
    aeApiState *state = eventLoop->apidata;

    if (mask & AE_READABLE) FD_SET(fd,&state->rfds);
    if (mask & AE_WRITABLE) FD_SET(fd,&state->wfds);
    return 0;
}

//����ض���fdɾ������д�¼�
static void aeApiDelEvent(aeEventLoop *eventLoop, int fd, int mask) {
    aeApiState *state = eventLoop->apidata;

    if (mask & AE_READABLE) FD_CLR(fd,&state->rfds);
    if (mask & AE_WRITABLE) FD_CLR(fd,&state->wfds);
}

//select��Pollʵ�ֹ���
static int aeApiPoll(aeEventLoop *eventLoop, struct timeval *tvp) {
    aeApiState *state = eventLoop->apidata;
    int retval, j, numevents = 0;

	//����һ�ݶ�д���ϣ���ֹ���޸�
    memcpy(&state->_rfds,&state->rfds,sizeof(fd_set));
    memcpy(&state->_wfds,&state->wfds,sizeof(fd_set));

	//�ڸ���֮��Ķ�д���Ͻ���select
    retval = select(eventLoop->maxfd+1,
                &state->_rfds,&state->_wfds,NULL,tvp);
    if (retval > 0) {
		//�������е�fd
        for (j = 0; j <= eventLoop->maxfd; j++) {
            int mask = 0;
			//���ϲ��ȡ����дmask
            aeFileEvent *fe = &eventLoop->events[j];

            if (fe->mask == AE_NONE) continue;
			//fd�ɶ�
            if (fe->mask & AE_READABLE && FD_ISSET(j,&state->_rfds))
                mask |= AE_READABLE;
			//fd��д
            if (fe->mask & AE_WRITABLE && FD_ISSET(j,&state->_wfds))
                mask |= AE_WRITABLE;

			//�ѿɶ����д��fd��¼���ϲ��fired����
            eventLoop->fired[numevents].fd = j;
            eventLoop->fired[numevents].mask = mask;
			//���Ӽ�����
            numevents++;
        }
    }
	//�����ܼ���
    return numevents;
}

static char *aeApiName(void) {
    return "select";
}
