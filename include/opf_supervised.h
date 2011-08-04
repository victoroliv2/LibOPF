#include "opf.h"

/* training function */
void supervised_training (subgraph * sg);

/* classification function: it simply
 * classifies samples from sg */
void supervised_classifying (subgraph * sgtrain, subgraph * sg);

/* learning function */
void supervised_learning (subgraph ** sgtrain, subgraph ** sgeval);

/* agglomerative learning function */
void supervised_agglomerative_learning (subgraph ** sgtrain, subgraph ** sgeval);
