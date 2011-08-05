// opf_PDF computation
void
opf_PDF (subgraph * sg)
{
  int i, nelems;
  double dist;
  float *value = AllocFloatArray (sg->node_n);
  set *adj = NULL;

  sg->K = (2.0 * (float) sg->df / 9.0);
  sg->dens_min = FLT_MAX;
  sg->dens_max = FLT_MIN;
  for (i = 0; i < sg->node_n; i++)
    {
      adj = sg->node[i].adj;
      value[i] = 0.0;
      nelems = 1;
      while (adj != NULL)
        {
          if (!use_precomputed_distance)
            dist =
              arc_weight (sg->node[i].feat, sg->node[adj->elem].feat,
                             sg->nfeats);
          else
            dist =
              opf_DistanceValue[sg->node[i].position][sg->
                                                      node[adj->
                                                           elem].position];
          value[i] += exp (-dist / sg->K);
          adj = adj->next;
          nelems++;
        }

      value[i] = (value[i] / (float) nelems);

      if (value[i] < sg->dens_min)
        sg->dens_min = value[i];
      if (value[i] > sg->dens_max)
        sg->dens_max = value[i];
    }

  //  printf("df=%f,K1=%f,K2=%f,dens_min=%f, dens_max=%f\n",sg->df,sg->K1,sg->K2,sg->dens_min,sg->dens_max);

  if (sg->dens_min == sg->dens_max)
    {
      for (i = 0; i < sg->node_n; i++)
        {
          sg->node[i].dens = DENS_MAX;
          sg->node[i].path_val = DENS_MAX - 1;
        }
    }
  else
    {
      for (i = 0; i < sg->node_n; i++)
        {
          sg->node[i].dens =
            ((float) (DENS_MAX - 1) * (value[i] - sg->dens_min) /
             (float) (sg->dens_max - sg->dens_min)) + 1.0;
          sg->node[i].path_val = sg->node[i].dens - 1;
        }
    }
  free (value);
}
