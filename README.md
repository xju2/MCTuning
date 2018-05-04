# Tuning the Monte Carlo Generators
The procedure of tuning the MC generator, which is Pythia~8, is the following:
   * Pythia 8 generates the truth events, which then will be analyzed by the pre-coded analysis selections in Rivet.
   * The outputs of the Rivet analysis are histogramed physical distributions, which will then be compared to experimental observations.
   * Use Professor to tune certain parameters in the Pythia 8 so that the generated events have a good agreement with the experimental observations, which is numerically evaluated by the chi-squared over number of degree of freedoms.
