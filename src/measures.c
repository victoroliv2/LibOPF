#include "common.h"
#include "measures.h"

// Normalized cut
double
subgraph_normalized_cut (struct subgraph * sg)
{
  int l, p, q;
  struct set *Saux;
  double ncut, dist;
  double *acumIC;                //acumulate weights inside each class
  double *acumEC;                //acumulate weights between the class and a distinct one

  ncut = 0.0;
  acumIC = alloc_double (sg->label_n);
  acumEC = alloc_double (sg->label_n);

  for (p = 0; p < sg->node_n; p++)
    {
      for (Saux = sg->node[p].adj; Saux != NULL; Saux = Saux->next)
        {
          q = Saux->elem;

          dist = subgraph_get_distance (sg, &sg->node[p], &sg->node[q]);

          if (dist > 0.0)
            {
              if (sg->node[p].label == sg->node[q].label)
                {
                  acumIC[sg->node[p].label] += 1.0 / dist;      // intra-class weight
                }
              else              // inter - class weight
                {
                  acumEC[sg->node[p].label] += 1.0 / dist;      // inter-class weight
                }
            }
        }
    }

  for (l = 0; l < sg->label_n; l++)
    {
      if (acumIC[l] + acumEC[l] > 0.0)
        ncut += (double) acumEC[l] / (acumIC[l] + acumEC[l]);
    }
  free (acumEC);
  free (acumIC);
  return (ncut);
}
