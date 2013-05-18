#include <iostream>
#include "smile.h"

using namespace std;

void CreateDBN(void);
void InferenceDBN(void);
//void UnrollDBN(void);

int main()
{
   //EnableXdslFormat();
   CreateDBN();
   InferenceDBN();
   //UnrollDBN();
   return DSL_OKAY;
}

void CreateDBN(void)
{
   // Initialize network and nodes.
   DSL_network theDBN;
   int rain = theDBN.AddNode(DSL_CPT ,"Rain");
   int umbrella = theDBN.AddNode(DSL_CPT ,"Umbrella");
   int area = theDBN.AddNode(DSL_CPT ,"Area");

   // Create and add statenames to [rain], [umbrella].
   DSL_idArray stateNames;
   stateNames.Add("True");
   stateNames.Add("False");
   theDBN.GetNode(rain)->Definition()->SetNumberOfOutcomes(stateNames);
   theDBN.GetNode(umbrella)->Definition()->SetNumberOfOutcomes(stateNames);

   // Create and add statenames to [area].
   stateNames.CleanUp();
   stateNames.Add("Pittsburgh");
   stateNames.Add("Sahara");
   theDBN.GetNode(area)->Definition()->SetNumberOfOutcomes(stateNames);

   // Add non-temporal arcs.
   theDBN.AddArc(area, rain);
   theDBN.AddArc(rain, umbrella);

   // Add non-temporal probabilities to nodes.
   DSL_doubleArray theProbs;

   // Add probabilities to [area].
   theProbs.SetSize(2);
   theProbs[0] = 0.5;
   theProbs[1] = 0.5;
   theDBN.GetNode(area)->Definition()->SetDefinition(theProbs);

   // Add probabilities to the initial CPT of [rain].
   theProbs.SetSize(4);
   theProbs[0] = 0.7;
   theProbs[1] = 0.3;
   theProbs[2] = 0.01;
   theProbs[3] = 0.99;
   theDBN.GetNode(rain)->Definition()->SetDefinition(theProbs);

   // Add probabilities to [umbrella].
   theProbs[0] = 0.9;
   theProbs[1] = 0.1;
   theProbs[2] = 0.2;
   theProbs[3] = 0.8;
   theDBN.GetNode(umbrella)->Definition()->SetDefinition(theProbs);

   // Set temporal types.
   theDBN.SetTemporalType(umbrella , dsl_plateNode);
   theDBN.SetTemporalType(rain, dsl_plateNode);
   // Add temporal arc.
   theDBN.AddTemporalArc(rain, rain, 1);

   // Add temporal probabilities to the first -order CPT of [rain].
   theProbs.SetSize(8);
   theProbs[0] = 0.7;
   theProbs[1] = 0.3;
   theProbs[2] = 0.3;
   theProbs[3] = 0.7;
   theProbs[4] = 0.001;
   theProbs[5] = 0.999;
   theProbs[6] = 0.01;
   theProbs[7] = 0.99;
   //theDBN.GetNode(rain)->TemporalDefinition()->SetTemporalProbabilities(theProbs , 1);

   ((DSL_cpt*)theDBN.GetNode(rain)->Definition())->SetTemporalProbabilities(1, theProbs); 


   // Write the DBN to a file.
   theDBN.WriteFile("dbn.xdsl");
}

void InferenceDBN(void)
{
   DSL_network theDBN;
   theDBN.ReadFile("dbn.xdsl");

   // Obtain the node handles.
   int rain = theDBN.FindNode("Rain");
   int umbrella = theDBN.FindNode("Umbrella");
   int area = theDBN.FindNode("Area");
   // Perform inference over a period of 8 days.
   theDBN.SetNumberOfSlices(8);
   // Set the evidence of the DBN.
   theDBN.GetNode(umbrella)->Value()->SetTemporalEvidence(0,1);
   theDBN.GetNode(umbrella)->Value()->SetTemporalEvidence(1,1);
   theDBN.GetNode(umbrella)->Value()->SetTemporalEvidence(2,1);
   theDBN.GetNode(umbrella)->Value()->SetTemporalEvidence(3,0);
   theDBN.GetNode(umbrella)->Value()->SetTemporalEvidence(4,1);
   theDBN.GetNode(umbrella)->Value()->SetTemporalEvidence(5,1);
   theDBN.GetNode(umbrella)->Value()->SetTemporalEvidence(6,1);
   // Do inference.
   theDBN.UpdateBeliefs();

   // Get beliefs.
   DSL_Dmatrix abeliefs;
   DSL_Dmatrix rbeliefs;
   abeliefs = *theDBN.GetNode(area)->Value()->GetMatrix();
   //theDBN.GetNode(rain)->Value()->GetTemporalValue(rbeliefs , 7);
   rbeliefs = *theDBN.GetNode(rain)->Value()->GetMatrix();
   // Print beliefs.
   DSL_idArray *stateNames;

   stateNames = theDBN.GetNode(area)->Definition()->GetOutcomesNames();
   cout << "Beliefs of [area]" << endl;
   cout << "  " << (*stateNames)[0] << "\t" << abeliefs[0] << endl;
   cout << "  " << (*stateNames)[1] << "\t" << abeliefs[1] << endl;
   cout << endl;

   stateNames = theDBN.GetNode(rain)->Definition()->GetOutcomesNames();
   cout << "Beliefs of [rain] tomorrow" << endl;
   cout << "  " << (*stateNames)[0] << "\t\t" << rbeliefs[0] << endl;
   cout << "  " << (*stateNames)[1] << "\t\t" << rbeliefs[1] << endl;
   cout << endl;
}