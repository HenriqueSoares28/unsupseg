
#include "gft_graph.h"

namespace gft
{
	namespace Graph
	{

		sGraph *Create(int nnodes, int outdegree, int *Wnodes)
		{
			sGraph *graph = NULL;
			int i;
			graph = (sGraph *)calloc(1, sizeof(sGraph));
			if (graph == NULL)
			{
				gft::Error((char *)MSG1, (char *)"Graph::Create");
			}
			graph->type = DISSIMILARITY;
			graph->nnodes = nnodes;
			graph->Wnodes = Wnodes;
			graph->nodes = (sGraphNode *)calloc(nnodes, sizeof(sGraphNode));
			if (graph->nodes == NULL)
			{
				gft::Error((char *)MSG1, (char *)"Graph::Create");
			}
			for (i = 0; i < nnodes; i++)
			{
				graph->nodes[i].outdegree = 0;
				graph->nodes[i].arraysize = outdegree;
				graph->nodes[i].id = i;
				graph->nodes[i].adjList = gft::AllocIntArray(outdegree);
				graph->nodes[i].Warcs = gft::AllocIntArray(outdegree);
			}
			return graph;
		}

		sGraph *Clone(sGraph *graph)
		{
			sGraph *clone;
			int p, q, i;
			int w;
			clone = Create(graph->nnodes, 10, NULL);
			clone->type = graph->type;
			if (graph->Wnodes != NULL)
			{
				clone->Wnodes = (int *)malloc(sizeof(int) * graph->nnodes);
				for (p = 0; p < graph->nnodes; p++)
				{
					clone->Wnodes[p] = graph->Wnodes[p];
				}
			}
			for (p = 0; p < graph->nnodes; p++)
			{
				clone->nodes[p].id = graph->nodes[p].id;
				for (i = 0; i < graph->nodes[p].outdegree; i++)
				{
					q = graph->nodes[p].adjList[i];
					w = graph->nodes[p].Warcs[i];
					AddDirectedEdge(clone, p, q, w);
				}
			}
			return clone;
		}

		sGraph *Clone(sImageGraph *sg)
		{
			sGraph *clone;
			sAdjRel *A;
			int ncols, nrows;
			int u_x, u_y, v_x, v_y;
			int p, q, i, n;
			int w;
			A = sg->A;
			ncols = sg->ncols;
			nrows = sg->nrows;
			n = ncols * nrows;
			clone = Create(n, A->n - 1, NULL);
			clone->type = sg->type;
			for (p = 0; p < n; p++)
			{
				u_x = p % ncols;
				u_y = p / ncols;

				for (i = 1; i < A->n; i++)
				{
					v_x = u_x + A->dx[i];
					v_y = u_y + A->dy[i];
					if (v_x >= 0 && v_x < ncols &&
						v_y >= 0 && v_y < nrows)
					{
						q = v_x + v_y * ncols;
						w = (sg->n_link[p])[i];
						AddDirectedEdge(clone, p, q, w);
					}
				}
			}
			return clone;
		}

		sGraph *Transpose(sGraph *graph)
		{
			sGraph *transp;
			int p, q, i;
			int w;
			transp = Create(graph->nnodes, 10, NULL);
			transp->type = graph->type;
			if (graph->Wnodes != NULL)
			{
				transp->Wnodes = (int *)malloc(sizeof(int) * graph->nnodes);
				for (p = 0; p < graph->nnodes; p++)
				{
					transp->Wnodes[p] = graph->Wnodes[p];
				}
			}
			for (p = 0; p < graph->nnodes; p++)
			{
				transp->nodes[p].id = graph->nodes[p].id;
				for (i = 0; i < graph->nodes[p].outdegree; i++)
				{
					q = graph->nodes[p].adjList[i];
					w = graph->nodes[p].Warcs[i];
					AddDirectedEdge(transp, q, p, w);
				}
			}
			return transp;
		}

		void Destroy(sGraph **graph)
		{
			sGraph *aux;
			int i;
			if (graph != NULL)
			{
				aux = *graph;
				if (aux != NULL)
				{
					if (aux->nodes != NULL)
					{
						for (i = 0; i < aux->nnodes; i++)
						{
							if (aux->nodes[i].adjList != NULL)
								gft::FreeIntArray(&aux->nodes[i].adjList);
							if (aux->nodes[i].Warcs != NULL)
								gft::FreeIntArray(&aux->nodes[i].Warcs);
						}
						free(aux->nodes);
					}
					free(aux);
					*graph = NULL;
				}
			}
		}

		int GetNumberOfArcs(sGraph *graph)
		{
			int p, Narcs = 0;
			for (p = 0; p < graph->nnodes; p++)
				Narcs += graph->nodes[p].outdegree;
			return Narcs;
		}

		int GetArcWeight(sGraph *graph, int src, int dest)
		{
			sGraphNode *s;
			s = &graph->nodes[src];
			int n, i;
			n = s->outdegree;
			for (i = 0; i < n; i++)
			{
				if (s->adjList[i] == dest)
				{
					//printf("s->Warcs %f\n",s->Warcs[i]);
					return s->Warcs[i];
				}
			}
			gft::Warning((char *)"Invalid node", (char *)"Graph::GetArcWeight");
			return -1;
		}

		int GetArcIndex(sGraph *graph, int src, int dest)
		{
			sGraphNode *s;
			s = &graph->nodes[src];
			int n, i;
			n = s->outdegree;
			for (i = 0; i < n; i++)
			{
				if (s->adjList[i] == dest)
				{
					return i;
				}
			}
			return NIL;
		}

		void AddEdge(sGraph *graph, int src, int dest, int w)
		{
			AddDirectedEdge(graph, src, dest, w);
			AddDirectedEdge(graph, dest, src, w);
		}

		void AddDirectedEdge(sGraph *graph, int src, int dest, int w)
		{
			sGraphNode *s;
			s = &graph->nodes[src];
			s->outdegree++;
			if (s->outdegree > s->arraysize)
			{
				s->adjList = (int *)realloc(s->adjList, sizeof(int) * s->outdegree);
				if (s->adjList == NULL)
					gft::Error((char *)MSG1, (char *)"Graph::AddDirectedEdge");
				s->Warcs = (int *)realloc(s->Warcs, sizeof(int) * s->outdegree);
				if (s->Warcs == NULL)
					gft::Error((char *)MSG1, (char *)"Graph::AddDirectedEdge");
				s->arraysize = s->outdegree;
			}
			s->adjList[s->outdegree - 1] = dest;
			s->Warcs[s->outdegree - 1] = w;
			//printf("W_graph %f\n",s->Warcs[s->outdegree-1]);
		}

		void UpdateEdge(sGraph *graph, int src, int dest, int w)
		{
			UpdateDirectedEdge(graph, src, dest, w);
			UpdateDirectedEdge(graph, dest, src, w);
		}

		void UpdateDirectedEdge(sGraph *graph, int src, int dest, int w)
		{
			sGraphNode *s;
			int i, find = false;
			s = &graph->nodes[src];

			for (i = 0; i < s->outdegree; i++)
			{
				if (dest == s->adjList[i])
				{
					s->Warcs[i] = w;
					find = true;
					break;
				}
			}
			if (!find)
				AddDirectedEdge(graph, src, dest, w);
		}

		void UpdateDirectedEdgeIfHigher(sGraph *graph, int src, int dest, int w)
		{
			sGraphNode *s;
			int i, find = false;
			s = &graph->nodes[src];
			for (i = 0; i < s->outdegree; i++)
			{
				if (dest == s->adjList[i])
				{
					if (s->Warcs[i] < w)
						s->Warcs[i] = w;
					find = true;
					break;
				}
			}
			if (!find)
				AddDirectedEdge(graph, src, dest, w);
		}

		void AddDirectedEdgeIfNonexistent(sGraph *graph, int src, int dest, int w)
		{
			sGraphNode *s;
			int i, find = false;
			s = &graph->nodes[src];
			for (i = 0; i < s->outdegree; i++)
			{
				if (dest == s->adjList[i])
				{
					find = true;
					break;
				}
			}
			if (!find)
				AddDirectedEdge(graph, src, dest, w);
		}

		void RemoveEdge(sGraph *graph, int src, int dest)
		{
			RemoveDirectedEdge(graph, src, dest);
			RemoveDirectedEdge(graph, dest, src);
		}

		void RemoveDirectedEdge(sGraph *graph, int src, int dest)
		{
			sGraphNode *s;
			int i, j;
			bool find = false;
			s = &graph->nodes[src];
			for (i = 0; i < s->outdegree; i++)
			{
				if (s->adjList[i] == dest)
				{
					j = i;
					find = true;
					break;
				}
			}
			if (find)
			{
				s->adjList[j] = s->adjList[s->outdegree - 1];
				s->Warcs[j] = s->Warcs[s->outdegree - 1];
				s->outdegree--;
			}
		}

		int GetMaximumArc(sGraph *graph)
		{
			int w, Wmax = INT_MIN;
			int p, i;
			for (p = 0; p < graph->nnodes; p++)
			{
				for (i = 0; i < graph->nodes[p].outdegree; i++)
				{
					w = graph->nodes[p].Warcs[i];
					if (w > Wmax && w != INT_MAX)
						Wmax = w;
				}
			}
			return Wmax;
		}

		void ChangeWeight(sGraph *graph,
						  int old_val, int new_val)
		{
			int p, i;
			for (p = 0; p < graph->nnodes; p++)
			{
				for (i = 0; i < graph->nodes[p].outdegree; i++)
				{
					if (graph->nodes[p].Warcs[i] == old_val)
						graph->nodes[p].Warcs[i] = new_val;
				}
			}
		}

		// Increasing transformation.
		void Pow(sGraph *graph,
				 int power, int max)
		{
			int w, Wmax = GetMaximumArc(graph);
			int p, i;
			long double v, m;
			if (Wmax <= 1)
				return;
			if (power <= 0)
				return;
			for (p = 0; p < graph->nnodes; p++)
			{
				for (i = 0; i < graph->nodes[p].outdegree; i++)
				{
					w = graph->nodes[p].Warcs[i];
					if (w == INT_MAX || w == 0)
						continue;
					v = (double)w;
					m = powl((v / Wmax), power);
					graph->nodes[p].Warcs[i] = MAX(ROUND(m * max), 1);
				}
			}
		}

		int Tarjan_aux(sGraph *graph, int *index, int *lowlink, int *label,
					   int i, int id, int *lb)
		{
			//pilhas usadas para simular a recursao:
			sStack *S_i = NULL;
			sStack *S_j = NULL;
			//pilha do algoritmo de Tarjan:
			sStack *S = NULL;
			int *IsInS = NULL;
			int j, t, k, tmp, flag = 0;
			enum action
			{
				f_in,
				f_out,
				f_end
			} act;

			IsInS = gft::AllocIntArray(graph->nnodes);
			S_i = gft::Stack::Create(graph->nnodes);
			S_j = gft::Stack::Create(graph->nnodes);
			S = gft::Stack::Create(graph->nnodes);
			act = f_in;
			//---------------------------
			j = 0;
			gft::Stack::Push(S_i, i);
			gft::Stack::Push(S_j, j);
			index[i] = id;
			lowlink[i] = id;
			id++;
			gft::Stack::Push(S, i);
			IsInS[i] = 1;

			//--------------------------
			do
			{
				switch (act)
				{
				case f_in:
					if (j < graph->nodes[i].outdegree)
					{
						k = graph->nodes[i].adjList[j];
						j = 0;
						i = k;
					}
					else
					{
						act = f_out;
						break;
					}

					if (index[i] != NIL)
					{
						flag = 1;
						if (IsInS[i])
						{
							//node.lowlink = min(node.lowlink, n.index);
							tmp = gft::Stack::Pop(S_i);
							if (lowlink[tmp] > index[i])
								lowlink[tmp] = index[i];
							gft::Stack::Push(S_i, tmp);
						}
						act = f_out;
					}
					else
					{
						index[i] = id;
						lowlink[i] = id;
						id++;
						gft::Stack::Push(S, i);
						IsInS[i] = 1;
						gft::Stack::Push(S_i, i);
						gft::Stack::Push(S_j, j);
					}
					break;
				case f_out:
					if (gft::Stack::IsEmpty(S_i))
						act = f_end;
					else
					{
						tmp = lowlink[i];
						i = gft::Stack::Pop(S_i);
						j = gft::Stack::Pop(S_j);

						if (lowlink[i] > tmp && !flag)
							lowlink[i] = tmp;

						flag = 0;
						j++;
						if (j < graph->nodes[i].outdegree)
						{
							gft::Stack::Push(S_i, i);
							gft::Stack::Push(S_j, j);
							act = f_in;
						}
						else
						{
							//if we are in the root of the component
							if (lowlink[i] == index[i])
							{
								do
								{
									if (gft::Stack::IsEmpty(S))
										break;
									t = gft::Stack::Pop(S);
									IsInS[t] = 0;
									label[t] = *lb;
								} while (t != i);
								(*lb)++;
							}
						}
					}
					break;
				case f_end:
					break;
				}
			} while (act != f_end);

			gft::FreeIntArray(&IsInS);
			gft::Stack::Destroy(&S_i);
			gft::Stack::Destroy(&S_j);
			gft::Stack::Destroy(&S);
			return id;
		}

		/*Tarjan's algorithm is a procedure for finding strongly connected components of a directed graph.*/
		int *Tarjan(sGraph *graph)
		{
			int *index = NULL;
			int *lowlink = NULL;
			int *label = NULL;
			int i, id = 0, lb = 1;

			index = gft::AllocIntArray(graph->nnodes);
			lowlink = gft::AllocIntArray(graph->nnodes);
			label = gft::AllocIntArray(graph->nnodes);
			for (i = 0; i < graph->nnodes; i++)
				index[i] = NIL;

			for (i = 0; i < graph->nnodes; i++)
			{
				if (index[i] != NIL)
					continue;
				id = Tarjan_aux(graph, index, lowlink, label, i, id, &lb);
			}

			gft::FreeIntArray(&index);
			gft::FreeIntArray(&lowlink);
			return label;
		}

		int *Tarjan(sGraph *graph, int p)
		{
			int *index = NULL;
			int *lowlink = NULL;
			int *label = NULL;
			int id = 0, lb = 1, i;

			index = gft::AllocIntArray(graph->nnodes);
			lowlink = gft::AllocIntArray(graph->nnodes);
			label = gft::AllocIntArray(graph->nnodes);
			for (i = 0; i < graph->nnodes; i++)
				index[i] = NIL;

			id = Tarjan_aux(graph, index, lowlink, label, p, id, &lb);

			lb = label[p];
			for (i = 0; i < graph->nnodes; i++)
			{
				if (label[i] != lb)
					label[i] = 0;
				else
					label[i] = 1;
			}

			gft::FreeIntArray(&index);
			gft::FreeIntArray(&lowlink);
			return label;
		}

		int *Tarjan(sGraph *graph, int *V, int n)
		{
			int *index = NULL;
			int *lowlink = NULL;
			int *label = NULL;
			int i, id = 0, lb = 1;

			index = gft::AllocIntArray(graph->nnodes);
			lowlink = gft::AllocIntArray(graph->nnodes);
			label = gft::AllocIntArray(graph->nnodes);
			for (i = 0; i < graph->nnodes; i++)
				index[i] = NIL;

			for (i = 0; i < n; i++)
			{
				if (index[V[i]] != NIL)
					continue;
				id = Tarjan_aux(graph, index, lowlink, label, V[i], id, &lb);
			}

			//----------------------------
			int *lb_map;
			int Lmax = 0, l;
			for (i = 0; i < graph->nnodes; i++)
			{
				if (label[i] == NIL)
					label[i] = 0;
				if (label[i] > Lmax)
					Lmax = label[i];
			}
			lb_map = gft::AllocIntArray(Lmax + 1);

			for (i = 0; i < n; i++)
				lb_map[label[V[i]]] = 1;

			l = 0;
			for (i = 1; i < Lmax + 1; i++)
			{
				if (lb_map[i] > 0)
				{
					l++;
					lb_map[i] = l;
				}
			}

			for (i = 0; i < graph->nnodes; i++)
				label[i] = lb_map[label[i]];

			gft::FreeIntArray(&lb_map);
			//----------------------------

			gft::FreeIntArray(&index);
			gft::FreeIntArray(&lowlink);
			return label;
		}

		void ChangeType(sGraph *graph, int type)
		{
			int w, Wmax;
			int p, i;
			Wmax = INT_MAX;
			for (p = 0; p < graph->nnodes; p++)
			{
				for (i = 0; i < graph->nodes[p].outdegree; i++)
				{
					w = graph->nodes[p].Warcs[i];
					if (w == 0 || w == INT_MAX)
						continue;
					if (w > Wmax)
						Wmax = w;
				}
			}
			for (p = 0; p < graph->nnodes; p++)
			{
				for (i = 0; i < graph->nodes[p].outdegree; i++)
				{
					w = graph->nodes[p].Warcs[i];
					if (w == INT_MAX)
						graph->nodes[p].Warcs[i] = 0;
					else if (w == 0)
						graph->nodes[p].Warcs[i] = INT_MAX;
					else
						graph->nodes[p].Warcs[i] = Wmax - w + 1;
				}
			}
			if (graph->type == DISSIMILARITY)
				graph->type = CAPACITY;
			else
				graph->type = DISSIMILARITY;
		}

		void HedgehogDigraph(sGraph *graph,
							 sImage32 *cost,
							 float theta,
							 float r)
		{
			sImage32 *gradx = NULL, *grady = NULL;
			sKernel *Kx, *Ky;
			sAdjRel *A;
			int ncols, nrows;
			int u_x, u_y, v_x, v_y;
			int i, p, q, n, q_min = 0;
			float mag, pq_x, pq_y, cosine;
			float inner_product1, inner_product2, min_inner_product;
			float Dd_px, Dd_py, Dd_qx, Dd_qy;
			float *Dpq;
			bool found;
			int newvalue_1, newvalue_2;
			if (graph->type == DISSIMILARITY)
			{
				newvalue_1 = 0;
				newvalue_2 = INT_MAX;
			}
			else
			{
				newvalue_1 = INT_MAX;
				newvalue_2 = 0;
			}
			A = AdjRel::Circular(r);
			ncols = cost->ncols;
			nrows = cost->nrows;
			n = ncols * nrows;

			cosine = cosf(theta * (PI / 180.0) + PI / 2.0);
			//--------------------
			Dpq = (float *)malloc(A->n * sizeof(float));
			for (i = 1; i < A->n; i++)
			{
				Dpq[i] = sqrtf(SQUARE(A->dx[i]) + SQUARE(A->dy[i]));
			}
			//--------------------

			/*
      if(sg->type == DISSIMILARITY)
	newval = 0;
      else
	newval = sg->Wmax;
      */

			Ky = Kernel::Make("3,3,-1.0,-2.0,-1.0,0.0,0.0,0.0,1.0,2.0,1.0");
			Kx = Kernel::Make("3,3,-1.0,0.0,1.0,-2.0,0.0,2.0,-1.0,0.0,1.0");
			gradx = LinearFilter(cost, Kx);
			grady = LinearFilter(cost, Ky);

			for (p = 0; p < n; p++)
			{
				u_x = p % ncols;
				u_y = p / ncols;

				Dd_px = gradx->data[p];
				Dd_py = grady->data[p];
				mag = sqrtf(SQUARE(Dd_px) + SQUARE(Dd_py));
				if (mag != 0.0)
				{
					Dd_px /= mag;
					Dd_py /= mag;
				}

				found = false;
				min_inner_product = 2.0;
				for (i = 1; i < A->n; i++)
				{
					v_x = u_x + A->dx[i];
					v_y = u_y + A->dy[i];
					if (v_x >= 0 && v_x < ncols &&
						v_y >= 0 && v_y < nrows)
					{
						q = v_x + v_y * ncols;

						Dd_qx = gradx->data[q];
						Dd_qy = grady->data[q];
						mag = sqrtf(SQUARE(Dd_qx) + SQUARE(Dd_qy));
						if (mag != 0.0)
						{
							Dd_qx /= mag;
							Dd_qy /= mag;
						}

						pq_x = A->dx[i] / Dpq[i];
						pq_y = A->dy[i] / Dpq[i];
						inner_product1 = Dd_px * pq_x + Dd_py * pq_y;
						inner_product2 = Dd_qx * pq_x + Dd_qy * pq_y;

						if (inner_product1 < min_inner_product)
						{
							min_inner_product = inner_product1;
							q_min = q;
						}

						if (inner_product1 <= cosine or inner_product2 <= cosine)
						{
							UpdateDirectedEdge(graph, p, q, newvalue_1);
							AddDirectedEdgeIfNonexistent(graph, q, p, newvalue_2);
							found = true;
						}
					}
				}

				if (!found)
				{
					UpdateDirectedEdge(graph, p, q_min, newvalue_1);
					AddDirectedEdgeIfNonexistent(graph, q_min, p, newvalue_2);
				}
			}

			free(Dpq);
			Destroy(&gradx);
			Destroy(&grady);
			Kernel::Destroy(&Kx);
			Kernel::Destroy(&Ky);
			AdjRel::Destroy(&A);
		}

		void LocalBandReduction(sGraph **graph,
								sImage32 *cost,
								int delta)
		{
			int p, q, w, wtp, i, cmin, s, t;
			int value_1, value_2;
			gft::sGraph *g, *ng;
			gft::sStack *S, *R;
			gft::sBMap *B, *T;
			if (graph == NULL)
				return;
			g = *graph;
			S = gft::Stack::Create(g->nnodes);
			R = gft::Stack::Create(g->nnodes);
			B = gft::BMap::Create(g->nnodes);
			T = gft::BMap::Create(g->nnodes);
			if (g->type == DISSIMILARITY)
			{
				value_1 = 0;
				value_2 = INT_MAX;
			}
			else
			{
				value_1 = INT_MAX;
				value_2 = 0;
			}

			for (p = 0; p < g->nnodes; p++)
			{
				cmin = INT_MAX;
				for (i = 0; i < g->nodes[p].outdegree; i++)
				{
					q = g->nodes[p].adjList[i];
					w = g->nodes[p].Warcs[i];
					if (w == value_1)
					{
						gft::Stack::Push(S, q);
						gft::BMap::Set1(B, q);
						gft::BMap::Set1(T, q);
						cmin = MIN(cmin, cost->data[q]);
					}
				}
				while (!gft::Stack::IsEmpty(S))
				{
					s = gft::Stack::Pop(S);
					gft::Stack::Push(R, s);
					for (i = 0; i < g->nodes[s].outdegree; i++)
					{
						t = g->nodes[s].adjList[i];
						w = g->nodes[s].Warcs[i];
						if (w == value_1)
						{
							if (gft::BMap::Get(T, t) == 1)
							{
								wtp = gft::Graph::GetArcWeight(g, t, p);
								if (wtp == value_2)
									gft::Graph::UpdateEdge(g, p, t, NIL);
							}
							else if (cost->data[t] >= cmin + delta && gft::BMap::Get(B, t) == 0)
							{
								gft::Stack::Push(S, t);
								gft::BMap::Set1(B, t);
							}
						}
					}
				}
				while (!gft::Stack::IsEmpty(R))
				{
					s = gft::Stack::Pop(R);
					gft::BMap::Set0(B, s);
					gft::BMap::Set0(T, s);
				}
			}

			ng = Create(g->nnodes, 10, NULL);
			ng->type = g->type;
			if (g->Wnodes != NULL)
			{
				ng->Wnodes = (int *)malloc(sizeof(int) * g->nnodes);
				for (p = 0; p < g->nnodes; p++)
					ng->Wnodes[p] = g->Wnodes[p];
			}
			for (p = 0; p < g->nnodes; p++)
			{
				ng->nodes[p].id = g->nodes[p].id;
				for (i = 0; i < g->nodes[p].outdegree; i++)
				{
					q = g->nodes[p].adjList[i];
					w = g->nodes[p].Warcs[i];
					if (w != NIL)
						AddDirectedEdge(ng, p, q, w);
				}
			}
			gft::Graph::Destroy(&g);
			*graph = ng;
			gft::Stack::Destroy(&S);
			gft::Stack::Destroy(&R);
			gft::BMap::Destroy(&B);
			gft::BMap::Destroy(&T);
		}

		void LocalBandConstraint(sGraph *graph,
								 sImage32 *cost,
								 int delta,
								 float r)
		{
			sAdjRel *A;
			int ncols, nrows;
			int u_x, u_y, v_x, v_y;
			int i, p, q, n;
			int newvalue_1, newvalue_2;
			if (graph->type == DISSIMILARITY)
			{
				newvalue_1 = 0;
				newvalue_2 = INT_MAX;
			}
			else
			{
				newvalue_1 = INT_MAX;
				newvalue_2 = 0;
			}

			A = AdjRel::Circular(r);
			ncols = cost->ncols;
			nrows = cost->nrows;
			n = ncols * nrows;

			for (p = 0; p < n; p++)
			{
				u_x = p % ncols;
				u_y = p / ncols;

				for (i = 1; i < A->n; i++)
				{
					v_x = u_x + A->dx[i];
					v_y = u_y + A->dy[i];
					if (v_x >= 0 && v_x < ncols &&
						v_y >= 0 && v_y < nrows)
					{
						q = v_x + v_y * ncols;
						if (cost->data[p] >= cost->data[q] + delta)
						{
							UpdateDirectedEdge(graph, p, q, newvalue_1);
							AddDirectedEdgeIfNonexistent(graph, q, p, newvalue_2);
						}
					}
				}
			}
			AdjRel::Destroy(&A);
		}

		/*Region Adjacency Graph (RAG)*/
		sGraph *RAG(sCImage *cimg,
					sImage32 *label)
		{
			sAdjRel *A;
			sGraph *graph = NULL;
			float *color[3];
			int nnodes, p, l, q, i, px, py, qx, qy, lp, lq;
			int *size;
			float w;
			nnodes = gft::Image32::GetMaxVal(label) + 1;
			color[0] = gft::AllocFloatArray(nnodes);
			color[1] = gft::AllocFloatArray(nnodes);
			color[2] = gft::AllocFloatArray(nnodes);
			size = gft::AllocIntArray(nnodes);
			if (color[0] == NULL ||
				color[1] == NULL ||
				color[2] == NULL ||
				size == NULL)
			{
				printf("Error\n");
				exit(1);
			}
			for (p = 0; p < label->n; p++)
			{
				l = label->data[p];
				size[l] += 1;
				color[0][l] += cimg->C[0]->data[p];
				color[1][l] += cimg->C[1]->data[p];
				color[2][l] += cimg->C[2]->data[p];
			}
			for (l = 0; l < nnodes; l++)
			{
				if (size[l] > 0)
				{
					color[0][l] /= size[l];
					color[1][l] /= size[l];
					color[2][l] /= size[l];
				}
				else
				{
					printf("Empty regions in RAG\n");
				}
			}
			//-------------
			graph = gft::Graph::Create(nnodes, 10, NULL);
			A = gft::AdjRel::Neighborhood_4();
			for (p = 0; p < label->n; p++)
			{
				lp = label->data[p];
				px = p % label->ncols;
				py = p / label->ncols;
				for (i = 1; i < A->n; i++)
				{
					qx = px + A->dx[i];
					qy = py + A->dy[i];
					if (gft::Image32::IsValidPixel(label, qx, qy))
					{
						q = qx + qy * label->ncols;
						lq = label->data[q];
						if (lp != lq)
						{
							w = sqrtf(SQUARE(color[0][lp] - color[0][lq]) +
									  SQUARE(color[1][lp] - color[1][lq]) +
									  SQUARE(color[2][lp] - color[2][lq]));
							gft::Graph::UpdateEdge(graph, lp, lq, ROUND(w));
						}
					}
				}
			}
			//-------------
			gft::FreeIntArray(&size);
			gft::FreeFloatArray(&color[0]);
			gft::FreeFloatArray(&color[1]);
			gft::FreeFloatArray(&color[2]);
			gft::AdjRel::Destroy(&A);
			return graph;
		}

		/*Region Adjacency Graph (RAG)*/
		sGraph *RAG(sCImage *cimg,
					sImage32 *label,
					int c1,
					int c2,
					float polarity)
		{
			sAdjRel *A;
			sGraph *graph = NULL;
			float *color[3];
			int nnodes, p, l, q, i, px, py, qx, qy, lp, lq;
			int *size;
			float w, wpq, wqp, wc1c2, wc1p, wc2p, wc1q, wc2q, rho;
			wc1c2 = sqrtf(SQUARE(gft::Color::Channel0(c1) - gft::Color::Channel0(c2)) +
						  SQUARE(gft::Color::Channel1(c1) - gft::Color::Channel1(c2)) +
						  SQUARE(gft::Color::Channel2(c1) - gft::Color::Channel2(c2)));
			wc1c2 /= 2.0;
			nnodes = gft::Image32::GetMaxVal(label) + 1;
			color[0] = gft::AllocFloatArray(nnodes);
			color[1] = gft::AllocFloatArray(nnodes);
			color[2] = gft::AllocFloatArray(nnodes);
			size = gft::AllocIntArray(nnodes);
			if (color[0] == NULL ||
				color[1] == NULL ||
				color[2] == NULL ||
				size == NULL)
			{
				printf("Error\n");
				exit(1);
			}
			for (p = 0; p < label->n; p++)
			{
				l = label->data[p];
				size[l] += 1;
				color[0][l] += cimg->C[0]->data[p];
				color[1][l] += cimg->C[1]->data[p];
				color[2][l] += cimg->C[2]->data[p];
			}
			for (l = 0; l < nnodes; l++)
			{
				if (size[l] > 0)
				{
					color[0][l] /= size[l];
					color[1][l] /= size[l];
					color[2][l] /= size[l];
				}
				else
				{
					printf("Empty regions in RAG\n");
				}
			}
			//-------------
			graph = gft::Graph::Create(nnodes, 10, NULL);
			A = gft::AdjRel::Neighborhood_4();
			for (p = 0; p < label->n; p++)
			{
				lp = label->data[p];
				px = p % label->ncols;
				py = p / label->ncols;

				wc1p = sqrtf(SQUARE(gft::Color::Channel0(c1) - color[0][lp]) +
							 SQUARE(gft::Color::Channel1(c1) - color[1][lp]) +
							 SQUARE(gft::Color::Channel2(c1) - color[2][lp]));
				wc2p = sqrtf(SQUARE(gft::Color::Channel0(c2) - color[0][lp]) +
							 SQUARE(gft::Color::Channel1(c2) - color[1][lp]) +
							 SQUARE(gft::Color::Channel2(c2) - color[2][lp]));

				for (i = 1; i < A->n; i++)
				{
					qx = px + A->dx[i];
					qy = py + A->dy[i];
					if (gft::Image32::IsValidPixel(label, qx, qy))
					{
						q = qx + qy * label->ncols;
						lq = label->data[q];
						if (lp != lq)
						{
							w = sqrtf(SQUARE(color[0][lp] - color[0][lq]) +
									  SQUARE(color[1][lp] - color[1][lq]) +
									  SQUARE(color[2][lp] - color[2][lq]));

							wc1q = sqrtf(SQUARE(gft::Color::Channel0(c1) - color[0][lq]) +
										 SQUARE(gft::Color::Channel1(c1) - color[1][lq]) +
										 SQUARE(gft::Color::Channel2(c1) - color[2][lq]));
							wc2q = sqrtf(SQUARE(gft::Color::Channel0(c2) - color[0][lq]) +
										 SQUARE(gft::Color::Channel1(c2) - color[1][lq]) +
										 SQUARE(gft::Color::Channel2(c2) - color[2][lq]));

							if (wc1p < wc1c2 && wc2q < wc1c2)
							{
								//rho = 1.0 - (wc1p+wc2q)/(2.0*wc1c2);
								//rho *= rho;
								rho = 1.0;
								wpq = w * (1.0 + polarity * rho);
								wqp = w * (1.0 - polarity * rho);
							}
							else if (wc1q < wc1c2 && wc2p < wc1c2)
							{
								//rho = 1.0 - (wc1q+wc2p)/(2.0*wc1c2);
								//rho *= rho;
								rho = 1.0;
								wpq = w * (1.0 - polarity * rho);
								wqp = w * (1.0 + polarity * rho);
							}
							else
							{
								rho = 1.0;
								wpq = wqp = w * (1.0 - polarity * rho);
							}
							gft::Graph::UpdateDirectedEdge(graph, lp, lq, ROUND(wpq));
							gft::Graph::UpdateDirectedEdge(graph, lq, lp, ROUND(wqp));
						}
					}
				}
			}
			//-------------
			gft::FreeIntArray(&size);
			gft::FreeFloatArray(&color[0]);
			gft::FreeFloatArray(&color[1]);
			gft::FreeFloatArray(&color[2]);
			gft::AdjRel::Destroy(&A);
			return graph;
		}

		/*Region Adjacency Graph (RAG)*/
		sGraph *RAG_lab(sCImage *cimg,
						sImage32 *label)
		{
			sCImage32f *cimg_lab;
			sAdjRel *A;
			sGraph *graph = NULL;
			float *color[3];
			int nnodes, p, l, q, i, px, py, qx, qy, lp, lq;
			int *size;
			float w, wpq, wqp;
			cimg_lab = CImage32f::RGB2Lab(cimg);
			nnodes = gft::Image32::GetMaxVal(label) + 1;
			color[0] = gft::AllocFloatArray(nnodes);
			color[1] = gft::AllocFloatArray(nnodes);
			color[2] = gft::AllocFloatArray(nnodes);
			size = gft::AllocIntArray(nnodes);
			if (color[0] == NULL ||
				color[1] == NULL ||
				color[2] == NULL ||
				size == NULL)
			{
				printf("Error\n");
				exit(1);
			}
			for (p = 0; p < label->n; p++)
			{
				l = label->data[p];
				size[l] += 1;
				color[0][l] += cimg_lab->C[0]->data[p];
				color[1][l] += cimg_lab->C[1]->data[p];
				color[2][l] += cimg_lab->C[2]->data[p];
			}
			for (l = 0; l < nnodes; l++)
			{
				if (size[l] > 0)
				{
					color[0][l] /= size[l];
					color[1][l] /= size[l];
					color[2][l] /= size[l];
				}
				else
				{
					printf("Empty regions in RAG\n");
				}
			}
			//-------------
			graph = gft::Graph::Create(nnodes, 10, NULL);
			A = gft::AdjRel::Neighborhood_4();
			for (p = 0; p < label->n; p++)
			{
				lp = label->data[p];
				px = p % label->ncols;
				py = p / label->ncols;

				for (i = 1; i < A->n; i++)
				{
					qx = px + A->dx[i];
					qy = py + A->dy[i];
					if (gft::Image32::IsValidPixel(label, qx, qy))
					{
						q = qx + qy * label->ncols;
						lq = label->data[q];
						if (lp != lq)
						{
							w = sqrtf(SQUARE(color[0][lp] - color[0][lq]) +
									  SQUARE(color[1][lp] - color[1][lq]) +
									  SQUARE(color[2][lp] - color[2][lq]));
							wpq = wqp = w;
							gft::Graph::UpdateDirectedEdge(graph, lp, lq, ROUND(wpq));
							gft::Graph::UpdateDirectedEdge(graph, lq, lp, ROUND(wqp));
						}
					}
				}
			}
			//-------------
			gft::FreeIntArray(&size);
			gft::FreeFloatArray(&color[0]);
			gft::FreeFloatArray(&color[1]);
			gft::FreeFloatArray(&color[2]);
			gft::AdjRel::Destroy(&A);
			gft::CImage32f::Destroy(&cimg_lab);
			return graph;
		}

		/*Region Adjacency Graph (RAG)*/
		sGraph *RAG_lab(sCImage *cimg,
						sImage32 *label,
						int c1,
						int c2,
						float polarity)
		{
			sCImage32f *cimg_lab;
			sAdjRel *A;
			sGraph *graph = NULL;
			float *color[3];
			int nnodes, p, l, q, i, px, py, qx, qy, lp, lq;
			int *size;
			float w, wpq, wqp, wc1c2, wc1p, wc2p, wc1q, wc2q, rho;
			double l1, a1, b1;
			double l2, a2, b2;
			cimg_lab = CImage32f::RGB2Lab(cimg);
			gft::Color::RGB2Lab(gft::Color::Channel0(c1),
								gft::Color::Channel1(c1),
								gft::Color::Channel2(c1),
								l1, a1, b1);
			gft::Color::RGB2Lab(gft::Color::Channel0(c2),
								gft::Color::Channel1(c2),
								gft::Color::Channel2(c2),
								l2, a2, b2);
			wc1c2 = sqrtf(SQUARE(l1 - l2) +
						  SQUARE(a1 - a2) +
						  SQUARE(b1 - b2));
			wc1c2 /= 2.0;
			nnodes = gft::Image32::GetMaxVal(label) + 1;
			color[0] = gft::AllocFloatArray(nnodes);
			color[1] = gft::AllocFloatArray(nnodes);
			color[2] = gft::AllocFloatArray(nnodes);
			size = gft::AllocIntArray(nnodes);
			if (color[0] == NULL ||
				color[1] == NULL ||
				color[2] == NULL ||
				size == NULL)
			{
				printf("Error\n");
				exit(1);
			}
			for (p = 0; p < label->n; p++)
			{
				l = label->data[p];
				size[l] += 1;
				color[0][l] += cimg_lab->C[0]->data[p];
				color[1][l] += cimg_lab->C[1]->data[p];
				color[2][l] += cimg_lab->C[2]->data[p];
			}
			for (l = 0; l < nnodes; l++)
			{
				if (size[l] > 0)
				{
					color[0][l] /= size[l];
					color[1][l] /= size[l];
					color[2][l] /= size[l];
				}
				else
				{
					printf("Empty regions in RAG\n");
				}
			}
			//-------------
			graph = gft::Graph::Create(nnodes, 10, NULL);
			A = gft::AdjRel::Neighborhood_4();
			for (p = 0; p < label->n; p++)
			{
				lp = label->data[p];
				px = p % label->ncols;
				py = p / label->ncols;

				wc1p = sqrtf(SQUARE(l1 - color[0][lp]) +
							 SQUARE(a1 - color[1][lp]) +
							 SQUARE(b1 - color[2][lp]));
				wc2p = sqrtf(SQUARE(l2 - color[0][lp]) +
							 SQUARE(a2 - color[1][lp]) +
							 SQUARE(b2 - color[2][lp]));

				for (i = 1; i < A->n; i++)
				{
					qx = px + A->dx[i];
					qy = py + A->dy[i];
					if (gft::Image32::IsValidPixel(label, qx, qy))
					{
						q = qx + qy * label->ncols;
						lq = label->data[q];
						if (lp != lq)
						{
							w = sqrtf(SQUARE(color[0][lp] - color[0][lq]) +
									  SQUARE(color[1][lp] - color[1][lq]) +
									  SQUARE(color[2][lp] - color[2][lq]));

							wc1q = sqrtf(SQUARE(l1 - color[0][lq]) +
										 SQUARE(a1 - color[1][lq]) +
										 SQUARE(b1 - color[2][lq]));
							wc2q = sqrtf(SQUARE(l2 - color[0][lq]) +
										 SQUARE(a2 - color[1][lq]) +
										 SQUARE(b2 - color[2][lq]));

							if (wc1p < wc1c2 && wc2q < wc1c2)
							{
								//rho = 1.0 - (wc1p+wc2q)/(2.0*wc1c2);
								//rho *= rho;
								rho = 1.0;
								wpq = w * (1.0 + polarity * rho);
								wqp = w * (1.0 - polarity * rho);
							}
							else if (wc1q < wc1c2 && wc2p < wc1c2)
							{
								//rho = 1.0 - (wc1q+wc2p)/(2.0*wc1c2);
								//rho *= rho;
								rho = 1.0;
								wpq = w * (1.0 - polarity * rho);
								wqp = w * (1.0 + polarity * rho);
							}
							else
							{
								rho = 1.0;
								wpq = wqp = w * (1.0 - polarity * rho);
							}
							gft::Graph::UpdateDirectedEdge(graph, lp, lq, ROUND(wpq));
							gft::Graph::UpdateDirectedEdge(graph, lq, lp, ROUND(wqp));
						}
					}
				}
			}
			//-------------
			gft::FreeIntArray(&size);
			gft::FreeFloatArray(&color[0]);
			gft::FreeFloatArray(&color[1]);
			gft::FreeFloatArray(&color[2]);
			gft::AdjRel::Destroy(&A);
			gft::CImage32f::Destroy(&cimg_lab);
			return graph;
		}

		/*Region Adjacency Graph (RAG)*/
		sGraph *RAG_lab_2(sCImage *cimg,
						  sImage32 *label,
						  int c1,
						  int c2,
						  float polarity)
		{
			sCImage32f *cimg_lab;
			sAdjRel *A;
			sGraph *graph = NULL;
			float *color[3];
			int nnodes, p, l, q, i, px, py, qx, qy, lp, lq;
			int *size;
			float w, wpq, wqp, wc1c2, wc1p, wc2p, wc1q, wc2q, rho;
			double l1, a1, b1;
			double l2, a2, b2;
			cimg_lab = CImage32f::RGB2Lab(cimg);
			gft::Color::RGB2Lab(gft::Color::Channel0(c1),
								gft::Color::Channel1(c1),
								gft::Color::Channel2(c1),
								l1, a1, b1);
			gft::Color::RGB2Lab(gft::Color::Channel0(c2),
								gft::Color::Channel1(c2),
								gft::Color::Channel2(c2),
								l2, a2, b2);
			wc1c2 = sqrtf(SQUARE(a1 - a2) +
						  SQUARE(b1 - b2));
			wc1c2 /= 2.0;
			nnodes = gft::Image32::GetMaxVal(label) + 1;
			color[0] = gft::AllocFloatArray(nnodes);
			color[1] = gft::AllocFloatArray(nnodes);
			color[2] = gft::AllocFloatArray(nnodes);
			size = gft::AllocIntArray(nnodes);
			if (color[0] == NULL ||
				color[1] == NULL ||
				color[2] == NULL ||
				size == NULL)
			{
				printf("Error\n");
				exit(1);
			}
			for (p = 0; p < label->n; p++)
			{
				l = label->data[p];
				size[l] += 1;
				color[0][l] += cimg_lab->C[0]->data[p];
				color[1][l] += cimg_lab->C[1]->data[p];
				color[2][l] += cimg_lab->C[2]->data[p];
			}
			for (l = 0; l < nnodes; l++)
			{
				if (size[l] > 0)
				{
					color[0][l] /= size[l];
					color[1][l] /= size[l];
					color[2][l] /= size[l];
				}
				else
				{
					printf("Empty regions in RAG\n");
				}
			}
			//-------------
			graph = gft::Graph::Create(nnodes, 10, NULL);
			A = gft::AdjRel::Neighborhood_4();
			for (p = 0; p < label->n; p++)
			{
				lp = label->data[p];
				px = p % label->ncols;
				py = p / label->ncols;

				wc1p = sqrtf(SQUARE(a1 - color[1][lp]) +
							 SQUARE(b1 - color[2][lp]));
				wc2p = sqrtf(SQUARE(a2 - color[1][lp]) +
							 SQUARE(b2 - color[2][lp]));

				for (i = 1; i < A->n; i++)
				{
					qx = px + A->dx[i];
					qy = py + A->dy[i];
					if (gft::Image32::IsValidPixel(label, qx, qy))
					{
						q = qx + qy * label->ncols;
						lq = label->data[q];
						if (lp != lq)
						{
							w = sqrtf(SQUARE(color[0][lp] - color[0][lq]) +
									  SQUARE(color[1][lp] - color[1][lq]) +
									  SQUARE(color[2][lp] - color[2][lq]));

							wc1q = sqrtf(SQUARE(a1 - color[1][lq]) +
										 SQUARE(b1 - color[2][lq]));
							wc2q = sqrtf(SQUARE(a2 - color[1][lq]) +
										 SQUARE(b2 - color[2][lq]));

							if (wc1p < wc1c2 && wc2q < wc1c2)
							{
								//rho = 1.0 - (wc1p+wc2q)/(2.0*wc1c2);
								//rho *= rho;
								rho = 1.0;
								wpq = w * (1.0 + polarity * rho);
								wqp = w * (1.0 - polarity * rho);
							}
							else if (wc1q < wc1c2 && wc2p < wc1c2)
							{
								//rho = 1.0 - (wc1q+wc2p)/(2.0*wc1c2);
								//rho *= rho;
								rho = 1.0;
								wpq = w * (1.0 - polarity * rho);
								wqp = w * (1.0 + polarity * rho);
							}
							else
							{
								rho = 1.0;
								wpq = wqp = w * (1.0 - polarity * rho);
							}
							gft::Graph::UpdateDirectedEdge(graph, lp, lq, ROUND(wpq));
							gft::Graph::UpdateDirectedEdge(graph, lq, lp, ROUND(wqp));
						}
					}
				}
			}
			//-------------
			gft::FreeIntArray(&size);
			gft::FreeFloatArray(&color[0]);
			gft::FreeFloatArray(&color[1]);
			gft::FreeFloatArray(&color[2]);
			gft::AdjRel::Destroy(&A);
			gft::CImage32f::Destroy(&cimg_lab);
			return graph;
		}

		/*Region Adjacency Graph (RAG)*/
		sGraph *RAG_lab_3(sCImage *cimg,
						  sImage32 *label,
						  int c1,
						  int c2,
						  float polarity)
		{
			sCImage32f *cimg_lab;
			sAdjRel *A;
			sGraph *graph = NULL;
			float *color[3];
			int nnodes, p, l, q, i, px, py, qx, qy, lp, lq;
			int *size;
			float w, wpq, wqp, wc1c2, wc1p, wc2p, wc1q, wc2q, rho;
			double l1, a1, b1;
			double l2, a2, b2;
			cimg_lab = CImage32f::RGB2Lab(cimg);
			gft::Color::RGB2Lab(gft::Color::Channel0(c1),
								gft::Color::Channel1(c1),
								gft::Color::Channel2(c1),
								l1, a1, b1);
			gft::Color::RGB2Lab(gft::Color::Channel0(c2),
								gft::Color::Channel1(c2),
								gft::Color::Channel2(c2),
								l2, a2, b2);
			wc1c2 = sqrtf(SQUARE(a1 - a2) +
						  SQUARE(b1 - b2));
			wc1c2 /= 2.0;
			nnodes = gft::Image32::GetMaxVal(label) + 1;
			color[0] = gft::AllocFloatArray(nnodes);
			color[1] = gft::AllocFloatArray(nnodes);
			color[2] = gft::AllocFloatArray(nnodes);
			size = gft::AllocIntArray(nnodes);
			if (color[0] == NULL ||
				color[1] == NULL ||
				color[2] == NULL ||
				size == NULL)
			{
				printf("Error\n");
				exit(1);
			}
			for (p = 0; p < label->n; p++)
			{
				l = label->data[p];
				size[l] += 1;
				color[0][l] += cimg_lab->C[0]->data[p];
				color[1][l] += cimg_lab->C[1]->data[p];
				color[2][l] += cimg_lab->C[2]->data[p];
			}
			for (l = 0; l < nnodes; l++)
			{
				if (size[l] > 0)
				{
					color[0][l] /= size[l];
					color[1][l] /= size[l];
					color[2][l] /= size[l];
				}
				else
				{
					printf("Empty regions in RAG\n");
				}
			}
			//-------------
			graph = gft::Graph::Create(nnodes, 10, NULL);
			A = gft::AdjRel::Neighborhood_4();
			for (p = 0; p < label->n; p++)
			{
				lp = label->data[p];
				px = p % label->ncols;
				py = p / label->ncols;

				wc1p = sqrtf(SQUARE(a1 - color[1][lp]) +
							 SQUARE(b1 - color[2][lp]));
				wc2p = sqrtf(SQUARE(a2 - color[1][lp]) +
							 SQUARE(b2 - color[2][lp]));

				for (i = 1; i < A->n; i++)
				{
					qx = px + A->dx[i];
					qy = py + A->dy[i];
					if (gft::Image32::IsValidPixel(label, qx, qy))
					{
						q = qx + qy * label->ncols;
						lq = label->data[q];
						if (lp != lq)
						{
							w = sqrtf(SQUARE(color[0][lp] - color[0][lq]) +
									  SQUARE(color[1][lp] - color[1][lq]) +
									  SQUARE(color[2][lp] - color[2][lq]));

							wc1q = sqrtf(SQUARE(a1 - color[1][lq]) +
										 SQUARE(b1 - color[2][lq]));
							wc2q = sqrtf(SQUARE(a2 - color[1][lq]) +
										 SQUARE(b2 - color[2][lq]));

							if (wc1p + wc2q < 2.0 * wc1c2)
							{
								//rho = 1.0 - (wc1p+wc2q)/(2.0*wc1c2);
								//rho *= rho;
								rho = 1.0;
								wpq = w * (1.0 + polarity * rho);
								wqp = w * (1.0 - polarity * rho);
							}
							else if (wc1q + wc2p < 2.0 * wc1c2)
							{
								//rho = 1.0 - (wc1q+wc2p)/(2.0*wc1c2);
								//rho *= rho;
								rho = 1.0;
								wpq = w * (1.0 - polarity * rho);
								wqp = w * (1.0 + polarity * rho);
							}
							else
							{
								rho = 1.0;
								wpq = wqp = w * (1.0 - polarity * rho);
							}
							gft::Graph::UpdateDirectedEdge(graph, lp, lq, ROUND(wpq));
							gft::Graph::UpdateDirectedEdge(graph, lq, lp, ROUND(wqp));
						}
					}
				}
			}
			//-------------
			gft::FreeIntArray(&size);
			gft::FreeFloatArray(&color[0]);
			gft::FreeFloatArray(&color[1]);
			gft::FreeFloatArray(&color[2]);
			gft::AdjRel::Destroy(&A);
			gft::CImage32f::Destroy(&cimg_lab);
			return graph;
		}

		sGraph *RAG(sImage32 *img,
					sImage32 *label)
		{
			sAdjRel *A;
			sGraph *graph = NULL;
			int nnodes, p, l, q, i, px, py, qx, qy, lp, lq;
			float *value = NULL;
			int *size;
			float w;
			nnodes = gft::Image32::GetMaxVal(label) + 1;
			value = gft::AllocFloatArray(nnodes);
			size = gft::AllocIntArray(nnodes);
			if (value == NULL || size == NULL)
			{
				printf("Error\n");
				exit(1);
			}
			for (p = 0; p < label->n; p++)
			{
				l = label->data[p];
				size[l] += 1;
				value[l] += img->data[p];
			}
			for (l = 0; l < nnodes; l++)
			{
				if (size[l] > 0)
					value[l] /= size[l];
				else
					printf("Empty regions in RAG\n");
			}
			//-------------
			graph = gft::Graph::Create(nnodes, 10, NULL);
			A = gft::AdjRel::Neighborhood_4();

			for (p = 0; p < label->n; p++)
			{
				lp = label->data[p];
				px = p % label->ncols;
				py = p / label->ncols;
				for (i = 1; i < A->n; i++)
				{
					qx = px + A->dx[i];
					qy = py + A->dy[i];
					if (gft::Image32::IsValidPixel(label, qx, qy))
					{
						q = qx + qy * label->ncols;
						lq = label->data[q];
						if (lp != lq)
						{
							w = fabsf(value[lp] - value[lq]);
							gft::Graph::UpdateEdge(graph, lp, lq, ROUND(w));
						}
					}
				}
			}
			//-------------
			gft::FreeIntArray(&size);
			gft::FreeFloatArray(&value);

			gft::AdjRel::Destroy(&A);
			return graph;
		}

		sGraph *RAG(sImage32 *img,
					sImage32 *label,
					float polarity)
		{
			sAdjRel *A;
			sGraph *graph = NULL;
			int nnodes, p, l, q, i, px, py, qx, qy, lp, lq;
			float *value = NULL;
			int *size;
			float w, wpq, wqp;
			nnodes = gft::Image32::GetMaxVal(label) + 1;
			value = gft::AllocFloatArray(nnodes);
			size = gft::AllocIntArray(nnodes);
			if (value == NULL || size == NULL)
			{
				printf("Error\n");
				exit(1);
			}
			for (p = 0; p < label->n; p++)
			{
				l = label->data[p];
				size[l] += 1;
				value[l] += img->data[p];
			}
			for (l = 0; l < nnodes; l++)
			{
				if (size[l] > 0)
					value[l] /= size[l];
				else
					printf("Empty regions in RAG\n");
			}
			//-------------
			graph = gft::Graph::Create(nnodes, 10, NULL);
			A = gft::AdjRel::Neighborhood_4();
			for (p = 0; p < label->n; p++)
			{
				lp = label->data[p];
				px = p % label->ncols;
				py = p / label->ncols;
				for (i = 1; i < A->n; i++)
				{
					qx = px + A->dx[i];
					qy = py + A->dy[i];
					if (gft::Image32::IsValidPixel(label, qx, qy))
					{
						q = qx + qy * label->ncols;
						lq = label->data[q];
						if (lp != lq)
						{
							w = fabsf(value[lp] - value[lq]);
							if (value[lp] > value[lq])
							{
								wpq = w * (1.0 + polarity);
								wqp = w * (1.0 - polarity);
							}
							else if (value[lp] < value[lq])
							{
								wpq = w * (1.0 - polarity);
								wqp = w * (1.0 + polarity);
							}
							else
							{
								wpq = wqp = w;
							}
                            printf("(%f,%f,%f) ",w,wpq,wqp);
							gft::Graph::UpdateDirectedEdge(graph, lp, lq, ROUND(wpq));
							gft::Graph::UpdateDirectedEdge(graph, lq, lp, ROUND(wqp));
						}
					}
				}
			}
			//-------------
			gft::FreeIntArray(&size);
			gft::FreeFloatArray(&value);
			gft::AdjRel::Destroy(&A);
			return graph;
		}

		sGraph *RAG(sScene32 *scn,
					sScene32 *label)
		{
			sAdjRel3 *A;
			sGraph *graph = NULL;
			int nnodes, p, l, q, i, lp, lq;
			float *value = NULL;
			int *size;
			float w;
			Voxel u, v;
			nnodes = gft::Scene32::GetMaximumValue(label) + 1;
			value = gft::AllocFloatArray(nnodes);
			size = gft::AllocIntArray(nnodes);
			printf("ENTROU gft  %d\n", nnodes);
			if (value == NULL || size == NULL)
			{
				printf("Error\n");
				exit(1);
			}
			for (p = 0; p < label->n; p++)
			{
				l = label->data[p];
				size[l] += 1;
				value[l] += scn->data[p];
			}
			for (l = 0; l < nnodes; l++)
			{
				if (size[l] > 0)
					value[l] /= size[l];
				else
					printf("Empty regions in RAG\n");
			}
			//-------------
			graph = gft::Graph::Create(nnodes, 10, NULL);
			A = gft::AdjRel3::Spheric(1.0);
			printf("Created Graphs n: %d\n", label->n);
			for (p = 0; p < label->n; p++)
			{
				lp = label->data[p];
				u.c.x = gft::Scene32::GetAddressX(label, p);
				u.c.y = gft::Scene32::GetAddressY(label, p);
				u.c.z = gft::Scene32::GetAddressZ(label, p);
				for (i = 1; i < A->n; i++)
				{
					v.v = u.v + A->d[i].v;
					if (gft::Scene32::IsValidVoxel(label, v))
					{
						q = gft::Scene32::GetVoxelAddress(label, v);
						lq = label->data[q];
						if (lp != lq)
						{
							w = fabsf(value[lp] - value[lq]);
							gft::Graph::UpdateEdge(graph, lp, lq, ROUND(w));
						}
					}
				}
			}
			//-------------
			if (size != NULL)
				gft::FreeIntArray(&size);
			if (value != NULL)
				gft::FreeFloatArray(&value);
			if (A != NULL)
				gft::AdjRel3::Destroy(&A);
			return graph;
		}

		sGraph *RAG(sScene32 *scn,
					sScene32 *label,
					float polarity)
		{
			sAdjRel3 *A;
			sGraph *graph = NULL;
			int nnodes, p, l, q, i, lp, lq;
			float *value = NULL;
			int *size;
			float w, wpq, wqp;
			Voxel u, v;
			nnodes = gft::Scene32::GetMaximumValue(label) + 1;
			value = gft::AllocFloatArray(nnodes);
			size = gft::AllocIntArray(nnodes);
			if (value == NULL || size == NULL)
			{
				printf("Error\n");
				exit(1);
			}
			for (p = 0; p < label->n; p++)
			{
				l = label->data[p];
				size[l] += 1;
				value[l] += scn->data[p];
			}
			for (l = 0; l < nnodes; l++)
			{
				if (size[l] > 0)
					value[l] /= size[l];
				else
					printf("Empty regions in RAG\n");
			}
			//-------------
			graph = gft::Graph::Create(nnodes, 10, NULL);
			A = gft::AdjRel3::Spheric(1.0);
			for (p = 0; p < label->n; p++)
			{
				lp = label->data[p];
				u.c.x = gft::Scene32::GetAddressX(label, p);
				u.c.y = gft::Scene32::GetAddressY(label, p);
				u.c.z = gft::Scene32::GetAddressZ(label, p);
				for (i = 1; i < A->n; i++)
				{
					v.v = u.v + A->d[i].v;
					if (gft::Scene32::IsValidVoxel(label, v))
					{
						q = gft::Scene32::GetVoxelAddress(label, v);
						lq = label->data[q];
						if (lp != lq)
						{
							w = fabsf(value[lp] - value[lq]);
							if (value[lp] > value[lq])
							{
								wpq = w * (1.0 + polarity);
								wqp = w * (1.0 - polarity);
							}
							else if (value[lp] < value[lq])
							{
								wpq = w * (1.0 - polarity);
								wqp = w * (1.0 + polarity);
							}
							else
							{
								wpq = wqp = w;
							}
							gft::Graph::UpdateDirectedEdge(graph, lp, lq, ROUND(wpq));
							gft::Graph::UpdateDirectedEdge(graph, lq, lp, ROUND(wqp));
						}
					}
				}
			}
			//-------------
			gft::FreeIntArray(&size);
			gft::FreeFloatArray(&value);
			gft::AdjRel3::Destroy(&A);
			return graph;
		}

		sGraph *Split(sGraph **G, int *label, int lb)
		{
			sGraph *G1 = NULL, *G2 = NULL;
			int *index = NULL;
			int p, q, nlb, i, j, w, lp, lq;

			if (G == NULL)
			{
				printf("Error\n");
				return NULL;
			}
			if ((*G)->Wnodes != NULL)
			{
				printf("Split does not support Wnodes\n");
				return NULL;
			}

			nlb = 0;
			for (p = 0; p < (*G)->nnodes; p++)
				if (label[p] == lb)
					nlb++;

			if ((*G)->nnodes == nlb || nlb == 0)
			{
				printf("Invalid split\n");
				return NULL;
			}

			G1 = Create(nlb,
						(*G)->nodes[0].arraysize, NULL);
			G2 = Create((*G)->nnodes - nlb,
						(*G)->nodes[0].arraysize, NULL);
			G1->type = (*G)->type;
			G2->type = (*G)->type;

			index = (int *)malloc((*G)->nnodes * sizeof(int));

			i = j = 0;
			for (p = 0; p < (*G)->nnodes; p++)
			{
				if (label[p] == lb)
				{
					G1->nodes[i].id = (*G)->nodes[p].id;
					index[p] = i;
					i++;
				}
				else
				{
					G2->nodes[j].id = (*G)->nodes[p].id;
					index[p] = j;
					j++;
				}
			}

			for (p = 0; p < (*G)->nnodes; p++)
			{
				lp = label[p];
				for (i = 0; i < (*G)->nodes[p].outdegree; i++)
				{
					q = (*G)->nodes[p].adjList[i];
					w = (*G)->nodes[p].Warcs[i];
					lq = label[q];
					if (lp == lb && lq == lb)
						AddDirectedEdge(G1, index[p], index[q], w);
					else if (lp != lb && lq != lb)
						AddDirectedEdge(G2, index[p], index[q], w);
				}
			}

			free(index);
			Destroy(G);
			*G = G2;
			return G1;
		}

	} // namespace Graph
} // namespace gft
