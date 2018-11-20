#include<stdio.h>
#include <time.h>
#include <stdlib.h>
#include "Graph.h"
#include "log.h"
#include "tool.h"

#define MAX_BUF (1 << 10)
static char _internal_buffer[MAX_BUF];

int weightedEdgeGetOther(struct edge_t *e, int v)
{
    if (e == NULL) {
        LOG("invalid argument : e == NULL!");
        return -1;
    }
    if (e->v == v) return e->w;
    if (e->w == v) return e->v;
    LOG("invalid argument : v=%d is not the vertices of edge", v);
    return -1;
}

int weightedEdgeIsValid(struct edge_t *e)
{
    return e->v >= 0 && e->w >= 0;
}

void weightedEdgeSet(struct edge_t *e, int v, int w, double weight, struct edge_t *next)
{
    e->v = v;
    e->w = w;
    e->weight = weight;
    e->next = next;
}

void weightedEdgeInvalidate(struct edge_t *e)
{
    e->v = e->w = -1;
    e->weight = 0;
    e->next = NULL;
}

int edgeWeightedGraphGetVertexCount(struct G *g)
{
    if (g == NULL) {
        LOG("invalid argument : g == NULL!");
        return -1;
    }
    return g->E;
}
int edgeWeightedGraphGetEdgeCount(struct G *g)
{
    if (g == NULL) {
        LOG("invalid argument : g == NULL!");
        return -1;
    }
    return g->V;
}

struct G* edgeWeightedGraphInit(int V) 
{
    if (V <= 0) {
        LOG("invalid argument : V=%d", V);
        return NULL;
    }

    struct G *g = NULL;
    if ((g = malloc(sizeof(struct G))) == NULL)
        goto err;

    g->V = V;
    g->E = 0;
    if ((g->adjs = malloc(sizeof(struct adj_list_t) * V)) == NULL)
        goto err_1;

    for (int i = 0; i < V; i++) {
        g->adjs[i].size = 0;
        g->adjs[i].first = NULL;
    }

    return g;

err_1:
    free(g);
err:
    return NULL;
}

struct G* edgeWeightedGraphRandomInit(int V, int E)
{
    if (V <= 0 || E < 0) {
        LOG("invalid argument : V=%d E=%d", V, E);
        return NULL;
    }

    srand((unsigned)time(NULL));

    struct G* g = NULL;
    if ((g = edgeWeightedGraphInit(V)) == NULL)
        return NULL;

    while (E--) 
        edgeWeightedGraphAddEdge(g, randomBetween(0, V), 
                                    randomBetween(0, V), 
                                    randomBetween(1, 100));
    return g;
}

void edgeWeightedGraphAddEdge(struct G *g, int v, int w, double weight)
{
    if (g == NULL || v < 0 || v >= g->V || w < 0 || w >= g->V) {
        LOG("invalid parameter: ");
        return;
    }
    struct edge_t *edgev = NULL, *edgew = NULL;
    if ((edgev = malloc(sizeof(struct edge_t))) == NULL) {
        LOG("malloc error for new vertex : {%d %d %.2f}", v, w, weight);
        return;
    }
    if ((edgew = malloc(sizeof(struct edge_t))) == NULL) {
        LOG("malloc error for new vertex : {%d %d %.2f}", v, w, weight);
        return;
    }

    edgev->v = v;
    edgev->w = w;
    edgev->weight = weight;

    edgew->v = v;
    edgew->w = w;
    edgew->weight = weight;

    g->E++;
    struct adj_list_t *adjv = &g->adjs[v];
    struct adj_list_t *adjw = &g->adjs[w];
    edgev->next = adjv->first;
    adjv->first = edgev;
    edgew->next = adjw->first;
    adjw->first = edgew;
}

void edgeWeightedGraphRelease(struct G** gg) 
{
    if (gg == NULL || *gg == NULL) {
        LOG("invalid parameter : g == NULL!");
        return;
    }
    struct G *g = *gg;
    for (int i = 0; i < g->V; i++) {
        for (struct edge_t *e = g->adjs[i].first; e != NULL; ) {
            struct edge_t *tmp = e;
            g->adjs[i].first = e->next;
            e = e->next;
            free(tmp);
        }
    }
    free(g->adjs);
    free(g);
    *gg = NULL;
}

const char *edgeWeightedGraphToString(struct G *g) 
{
    if (g == NULL) {
        _internal_buffer[0] = 0;
        return _internal_buffer;
    }

#define APPEND(_format_, ...) (len += snprintf(_internal_buffer + len, MAX_BUF - len, _format_, ##__VA_ARGS__));
    ssize_t len = 0;
    APPEND("V = %d\n", g->V);
    APPEND("E = %d\n", g->E);
    for (int i = 0; i < g->V; i++) {
        APPEND("%d :", i);
        for (struct edge_t *cur = g->adjs[i].first; cur != NULL; cur = cur->next) 
            APPEND(" { %d %d %.0f }", cur->v, cur->w, cur->weight);
        APPEND("\n");
    }
    _internal_buffer[len] = 0;
    return _internal_buffer;
#undef APPEND
}
