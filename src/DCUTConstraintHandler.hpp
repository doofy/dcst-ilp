#ifndef DCUT_HANDLER_H
#define DCUT_HANDLER_H

#include <objscip/objscip.h>

class DCUTConstraintHandler : public scip::ObjConshdlr {

  public:
    DCUTConstraintHandler( SCIP*  scip );
    ~DCUTConstraintHandler();

    //----------------------------------------
    /// Frees specific constraint data.
    virtual SCIP_RETCODE scip_delete(
        SCIP*              scip,               /**< SCIP data structure */
        SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
        SCIP_CONS*         cons,               /**< the constraint belonging to the constraint data */
        SCIP_CONSDATA**    consdata            /**< pointer to the constraint data to free */
        );

    /// Transforms constraint data into data belonging to the transformed problem.
    virtual SCIP_RETCODE scip_trans(
        SCIP*              scip,               /**< SCIP data structure */
        SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
        SCIP_CONS*         sourcecons,         /**< source constraint to transform */
        SCIP_CONS**        targetcons          /**< pointer to store created target constraint */
        );


    //----------------------------------------

    /// Separation method for LP solution.
    virtual SCIP_RETCODE scip_sepalp(
        SCIP*              scip,               /**< SCIP data structure */
        SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
        SCIP_CONS**        conss,              /**< array of constraints to process */
        int                nconss,             /**< number of constraints to process */
        int                nusefulconss,       /**< number of useful (non-obsolete) constraints to process */
        SCIP_RESULT*       result              /**< pointer to store the result of the separation call */
        );

    /// Separation method for heuristic and other solution.
    virtual SCIP_RETCODE scip_sepasol(
        SCIP*              scip,               /**< SCIP data structure */
        SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
        SCIP_CONS**        conss,              /**< array of constraints to process */
        int                nconss,             /**< number of constraints to process */
        int                nusefulconss,       /**< number of useful (non-obsolete) constraints to process */
        SCIP_SOL*          sol,                /**< primal solution that should be separated */
        SCIP_RESULT*       result              /**< pointer to store the result of the separation call */
        );

    /// Constraint enforcing method of constraint handler for LP solutions.
    virtual SCIP_RETCODE scip_enfolp(
        SCIP*              scip,               /**< SCIP data structure */
        SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
        SCIP_CONS**        conss,              /**< array of constraints to process */
        int                nconss,             /**< number of constraints to process */
        int                nusefulconss,       /**< number of useful (non-obsolete) constraints to process */
        SCIP_Bool          solinfeasible,      /**< was the solution already declared infeasible by a constraint handler? */
        SCIP_RESULT*       result              /**< pointer to store the result of the enforcing call */
        );

    /// Constraint enforcing method of constraint handler for pseudo solutions.
    virtual SCIP_RETCODE scip_enfops(
        SCIP*              scip,               /**< SCIP data structure */
        SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
        SCIP_CONS**        conss,              /**< array of constraints to process */
        int                nconss,             /**< number of constraints to process */
        int                nusefulconss,       /**< number of useful (non-obsolete) constraints to process */
        SCIP_Bool          solinfeasible,      /**< was the solution already declared infeasible by a constraint handler? */
        SCIP_Bool          objinfeasible,      /**< is the solution infeasible anyway due to violating lower objective bound? */
        SCIP_RESULT*       result              /**< pointer to store the result of the enforcing call */
        );

    /// Feasibility check method of constraint handler for primal solutions.
    virtual SCIP_RETCODE scip_check(
        SCIP*              scip,               /**< SCIP data structure */
        SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
        SCIP_CONS**        conss,              /**< array of constraints to process */
        int                nconss,             /**< number of constraints to process */
        SCIP_SOL*          sol,                /**< the solution to check feasibility for */
        SCIP_Bool          checkintegrality,   /**< has integrality to be checked? */
        SCIP_Bool          checklprows,        /**< have current LP rows to be checked? */
        SCIP_Bool          printreason,        /**< should the reason for the violation be printed? */
        SCIP_RESULT*       result              /**< pointer to store the result of the feasibility checking call */
        );


    /// Variable rounding lock method of constraint handler.
    virtual SCIP_RETCODE scip_lock(
        SCIP*              scip,               /**< SCIP data structure */
        SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
        SCIP_CONS*         cons,               /**< the constraint that should lock rounding of its variables, or NULL if the
                                                *   constraint handler does not need constraints */
        int                nlockspos,          /**< no. of times, the roundings should be locked for the constraint */
        int                nlocksneg           /**< no. of times, the roundings should be locked for the constraint's negation */
        );



    /// Constraint display method of constraint handler.
    virtual SCIP_RETCODE scip_print(
        SCIP*              scip,               /**< SCIP data structure */
        SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
        SCIP_CONS*         cons,               /**< the constraint that should be displayed */
        FILE*              file                /**< the text file to store the information into */
        );

};


/// Creates and captures a directed cut constraint.
SCIP_RETCODE SCIPcreateConsDCUTConstraint(
    SCIP*                 scip,               /**< SCIP data structure */
    SCIP_CONS**           cons,               /**< pointer to hold the created constraint */
    const char*           name,               /**< name of constraint */
    Graph&                data,               /**< the underlying STP data */
    SCIP_Bool             initial,            /**< should the LP relaxation of constraint be in the initial LP? */
    SCIP_Bool             separate,           /**< should the constraint be separated during LP processing? */
    SCIP_Bool             enforce,            /**< should the constraint be enforced during node processing? */
    SCIP_Bool             check,              /**< should the constraint be checked for feasibility? */
    SCIP_Bool             propagate,          /**< should the constraint be propagated during node processing? */
    SCIP_Bool             local,              /**< is constraint only valid locally? */
    SCIP_Bool             modifiable,         /**< is constraint modifiable (subject to column generation)? */
    SCIP_Bool             dynamic,            /**< is constraint dynamic? */
    SCIP_Bool             removable           /**< should the constraint be removed from the LP due to aging or cleanup? */
    );

#endif
