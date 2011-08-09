#include "common.h"
#include "measures.h"

// Normalized cut
float
subgraph_normalized_cut (struct subgraph * sg)
{
  int l, p, q;
  struct set *Saux;
  float ncut, dist;
  float *acumIC;                //acumulate weights inside each class
  float *acumEC;                //acumulate weights between the class and a distinct one

  ncut = 0.0;
  acumIC = alloc_float (sg->label_n);
  acumEC = alloc_float (sg->label_n);

  for (p = 0; p < sg->node_n; p++)
    {
      for (Saux = sg->node[p].adj; Saux != NULL; Saux = Saux->next)
        {
          q = Saux->elem;

          if (!sg->use_precomputed_distance)
            dist = sg->arc_weight (sg->node[p].feat, sg->node[q].feat, sg->feat_n);
          else
            dist = sg->distance_value[sg->node[p].position][sg->node[q].position];

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
        ncut += (float) acumEC[l] / (acumIC[l] + acumEC[l]);
    }
  free (acumEC);
  free (acumIC);
  return (ncut);
}
