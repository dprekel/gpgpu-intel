# Particle Mesh Ewald (PME)
This kernel was extracted from the GROMACS molecular simulation package for testing purposes.

## Molecular Dynamics Simulations
Molecular Dynamics simulations are very common in the fields of chemistry and biochemistry.
It is an iterative method that samples the phase space of a system of atoms/molecules by numerically integrating Newtons equations of motion:

```math
m_i\frac{\mathrm{d}^2}{\mathrm{d}t^2}\vec{R}_{i} = \nabla_{i}E_{\mathrm{tot}}({\vec{R}_i})
```

The potential energy $E_{\mathrm{tot}}$ can be obtained by solving the electronic Schrödinger equation within the Born-Oppenheimer approximation. 
In most cases this is too computationally demanding though because it must be calculated for each time step. 
So empirical force fields have been developed that roughly approximate the real potential energy surface (PES).
Empirical force fields split up the potential energy into bonded (covalent) and nonbonded (electrostatic and van-der-Waals) terms, a separation that doesn't exist within a strict quantum mechanical treatment.
Usually, they have the following form:

```math
\begin{aligned}
E_{\mathrm{tot}} =& \sum_{\mathrm{bonds}}\frac{1}{2}k_{\alpha}\big(d_{\alpha}-d_{\alpha,e}\big)^2\\
+&\sum_{\mathrm{angles}}\frac{1}{2}\tilde{k}_{\alpha}\big(\theta_{\alpha}-\theta_{\alpha,e}\big)^2\\
+&\sum_{\mathrm{dihedral}}\frac{1}{2}V_{\alpha}\big(1+\cos(n_{\alpha}\omega_{\alpha}-\omega_{\alpha,e})\big)^2\\
+&\sum_{i=1}^{N}\sum_{j>i}^{N}\frac{q_iq_j}{4\pi\varepsilon_0\vert\vec{R}_i-\vec{R}_j\vert}\\
+&\sum_{i=1}^{N}\sum_{j>i}^{N}4\varepsilon_{ij}\Bigg[\bigg(\frac{\sigma}{R}\bigg)^{12}-\bigg(\frac{\sigma}{R}\bigg)^6\Bigg]
\end{aligned}
```
