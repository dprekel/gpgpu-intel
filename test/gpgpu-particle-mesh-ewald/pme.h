#pragma once

/*! \brief Nbnxm electrostatic GPU kernel flavors.
 *
 *  Types of electrostatics implementations available in the GPU non-bonded
 *  force kernels. These represent both the electrostatics types implemented
 *  by the kernels (cut-off, RF, and Ewald - a subset of what's defined in
 *  enums.h) as well as encode implementation details analytical/tabulated
 *  and single or twin cut-off (for Ewald kernels).
 *  Note that the cut-off and RF kernels have only analytical flavor and unlike
 *  in the CPU kernels, the tabulated kernels are ATM Ewald-only.
 *
 *  The row-order of pointers to different electrostatic kernels defined in
 *  nbnxn_cuda.cu by the nb_*_kfunc_ptr function pointer table
 *  should match the order of enumerated types below.
 */
enum class ElecType : int {
    Cut,          //!< Plain cut-off
    RF,           //!< Reaction field
    EwaldTab,     //!< Tabulated Ewald with single cut-off
    EwaldTabTwin, //!< Tabulated Ewald with twin cut-off
    EwaldAna,     //!< Analytical Ewald with single cut-off
    EwaldAnaTwin, //!< Analytical Ewald with twin cut-off
    Count         //!< Number of valid values
};


/*! \brief Nbnxm VdW GPU kernel flavors.
 *
 * The enumerates values correspond to the LJ implementations in the GPU non-bonded
 * kernels.
 *
 * The column-order of pointers to different electrostatic kernels defined in
 * nbnxn_cuda_ocl.cpp/.cu by the nb_*_kfunc_ptr function pointer table
 * should match the order of enumerated types below.
 */
enum class VdwType : int {
    Cut,         //!< Plain cut-off
    CutCombGeom, //!< Cut-off with geometric combination rules
    CutCombLB,   //!< Cut-off with Lorentz-Berthelot combination rules
    FSwitch,     //!< Smooth force switch
    PSwitch,     //!< Smooth potential switch
    EwaldGeom,   //!< Ewald with geometric combination rules
    EwaldLB,     //!< Ewald with Lorentz-Berthelot combination rules
    Count        //!< Number of valid values
};

struct shift_consts {
    float c2;
    float c3;
    float cpot;
};

struct switch_consts {
    float c3;
    float c4;
    float c5;
};

struct cl_nbparam_params {
    // type of electrostatics
    enum ElecType elecType;
    // type of van der Waals impl.
    enum VdwType vdwType;
    // charge multiplication factor
    float epsfac;
    // Reaction-field/plain cutoff electrostatics const.
    float c_rf;
    // Reaction-field electrostatics constant
    float two_k_rf;
    // Ewald/PME parameter
    float ewald_beta;
    // Ewald/PME correction term subtracted from the direct-space potential
    float sh_ewald;
    // LJ-Ewald/PME correction term added to the correction potential
    float sh_lj_ewald;
    // LJ-Ewald/PME coefficient
    float ewaldcoeff_lj;
    // Coulomb cut-off squared
    float rcoulomb_sq;
    // VdW cut-off squared
    float rvdw_sq;
    // VdW switched cut-off
    float rvdw_switch;
    // Full, outer pair-list cut-off squared
    float rlistOuter_sq;
    // Inner, dynamic pruned pair-list cut-off squared
    float rlistInner_sq;
    // VdW shift dispersion constants
    shift_consts dispersion_shift;
    // VdW shift repulsion constants
    shift_consts repulsion_shift;
    // VdW switch constants
    switch_consts vdw_switch;
    /* Ewald Coulomb force table data - accessed through texture memory */
    // table scale/spacing
    float coulomb_tab_scale;
};





