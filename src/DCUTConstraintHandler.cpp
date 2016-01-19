#include "graph.hpp"

#include <iostream>
#include <queue>
#include <string>

#include "DCUTConstraintHandler.hpp"

#include <boost/config.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graph_traits.hpp>

struct SCIP_ConsData {
  Graph& data;
};

static const char* HANDLER_NAME = "DCUT";
static const char* HANDLER_DESC = "directed cut constraints";

SCIP_Bool ownGraphToBoost(Graph& ownG, Graph& g,
    boost::CapacityMap capacity,
    ReverseEdgeMap reverse_edge
    ) {

  std::vector<vertex_descriptor> verts;

  for (std::vector<Node>::iterator node = ownG.nodes.begin(); node != ownG.nodes.end(); node++) {
    verts.push_back(add_vertex(g));
  }

  for (std::vector<Edge>::iterator edge = ownG.edges.begin(); edge != ownG.edges.end(); edge++) {
    if (!edge->partOfSolution) continue;
    long head = edge->from;
    long tail = edge->to;
    long cap = edge->cost;

    edge_descriptor e1, e2;
    bool in1, in2;
    boost::tie(e1, in1) = boost:add_edge(verts[tail], verts[head], g);
    boost::tie(e2, in2) = boost:add_edge(verts[head], verts[tail], g);
    if (!in1 || !in2) {
      std::cerr << "unable to add edge (" << head << "," << tail << ")"	<< std::endl;
      return false;
    }
    capacity[e1] = cap;
    capacity[e2] = 0;
    reverse_edge[e1] = e2;
    reverse_edge[e2] = e1;
  }

  return true;
}

SCIP_Bool findDCUT (SCIP* scip, Graph& graph, long s, long t) {
  typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> Traits;
  typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
	  boost::property<boost::vertex_name_t, std::string>,
	  boost::property<boost::edge_capacity_t, long,
	  boost::property<boost::edge_residual_capacity_t, long,
	  boost::property<boost::edge_reverse_t, Traits::edge_descriptor> > >
	    > Graph;

  Graph g;

  boost::property_map<boost::Graph, boost::edge_capacity_t>::type capacity = get(edge_capacity, g);
  boost::property_map<boost::Graph, boost::edge_reverse_t>::type rev = get(edge_reverse, g);
  boost::property_map<boost::Graph, boost::edge_residual_capacity_t>::type residual_capacity = get(edge_residual_capacity, g);

  Traits::vertex_descriptor s, t;
  //typename graph_traits<Graph>::vertex_descriptor& src;
  //typename graph_traits<Graph>::vertex_descriptor& sink;
  //TODO Set s and t
  if (!ownGraphToBoost(graph, g, capacity, rev, s, t)) {
    std::cerr << "OWN GRAPH TO BOOST FAILED!!!!!!!!!!!" << std::endl;
    return false;
  }

  long flow;
  flow = boost::push_relabel_max_flow(g, s, t);

  std::cout << "s: " << s << "   t: " << t << "   flow: " << flow << std::endl;

  boost::graph_traits<boost::Graph>::vertex_iterator u_iter, u_end;
  boost::graph_traits<boost::Graph>::out_edge_iterator ei, e_end;
  for (boost::tie(u_iter, u_end) = boost::vertices(g); u_iter != u_end; ++u_iter)
    for (boost::tie(ei, e_end) = out_edges(*u_iter, g); ei != e_end; ++ei)
      std::cout << "f " << *u_iter << " " << target(*ei, g) << " "
	<< (capacity[*ei] - residual_capacity[*ei]) << std::endl;

  std::cout << std::endl;

  return false;
}

//----------------------------------------
  SCIP_RETCODE sepaDirectedSteinerCut
( SCIP*              scip,               /**< SCIP data structure */
  SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
  SCIP_CONS**        conss,              /**< array of constraints to process */
  int                nconss,             /**< number of constraints to process */
  int                nusefulconss,       /**< number of useful (non-obsolete) constraints to process */
  SCIP_SOL*          sol,                /**< primal solution that should be separated */
  SCIP_RESULT*       result              /**< pointer to store the result of the separation call */
  )
{
  assert(result != NULL);
  *result = SCIP_DIDNOTFIND;

  int cuts  = 0;
  int flows = 0;

  for( int c = 0; c < nusefulconss; ++c ) {

    // get all required structures
    SCIP_CONSDATA*   consdata = SCIPconsGetData(conss[c]);;
    assert(consdata != NULL);
    const STPData*   data     = consdata->data;
    assert(data != NULL);

    const Digraph& g = data->graph();
    const int      n = g.num_nodes();
    const int      m = g.num_edges();

    const vector<int>& terminals = data->terminals();
    const int          t         = terminals.size();

    vector<int> cap     ( m );
    vector<int> flow    ( m );
    vector<int> inedges ( n, -1 );

    /**************
     * BEGIN_TODO:  SEPARATION FOR (FRACTIONAL) SOLUTIONS
     * Check if the (fractional) capacities satisfy all directed Steiner cuts constraints
     *
     * To create and add a violated cut, use the following template scheme:

     * SCIP_ROW* row;
     * SCIP_CALL( SCIPcreateEmptyRow(scip, &row, "new_row_name",
     *                               1.0,                // lhs
     *                               SCIPinfinity(scip), // rhs
     *                               FALSE, FALSE, FALSE) );
     * SCIP_CALL( SCIPcacheRowExtensions(scip, row) );
     * SCIP_CALL( SCIPaddVarToRow(scip, row, variable1, coefficient1) );
     * SCIP_CALL( SCIPaddVarToRow(scip, row, variable2, coefficient2) );
     * ...
     * SCIP_CALL( SCIPflushRowExtensions(scip, row) );
     * SCIP_CALL( SCIPaddCut(scip, sol, row, FALSE) );
     * *result = SCIP_SEPARATED;
     * SCIP_CALL( SCIPreleaseRow(scip, &row) );
     *
     *
     * You can access the fractional solution values as shown in the comment in findDirectedSteinerCut.
     * Because this method is called also for fractional solution candidate vectors, you must
     * turn the fractional values into integer capacities via scaling and rounding in an appropriate way.
     * (Hint: This may have a big effect on the solution times.)
     *
     * You should also try to generate many different (orthogonal) violated ineqaulities in one single call
     * to this function.
     **************/

    /**************
     * SOLUTION:  SEPARATION FOR (FRACTIONAL) SOLUTIONS
     **************/

    /**************
     * END_TODO:  SEPARATION FOR (FRACTIONAL) SOLUTIONS
     **************/
  }
  return SCIP_OKAY;
}



//----------------------------------------
SCIP_RETCODE DCUTConstraintHandler::scip_sepalp(
    SCIP*              scip,               /**< SCIP data structure */
    SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
    SCIP_CONS**        conss,              /**< array of constraints to process */
    int                nconss,             /**< number of constraints to process */
    int                nusefulconss,       /**< number of useful (non-obsolete) constraints to process */
    SCIP_RESULT*       result              /**< pointer to store the result of the separation call */
    )
{
  SCIP_CALL( sepaDirectedSteinerCut(scip, conshdlr, conss, nconss, nusefulconss, NULL, result) );
  return SCIP_OKAY;
}

//----------------------------------------
SCIP_RETCODE DCUTConstraintHandler::scip_sepasol(
    SCIP*              scip,               /**< SCIP data structure */
    SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
    SCIP_CONS**        conss,              /**< array of constraints to process */
    int                nconss,             /**< number of constraints to process */
    int                nusefulconss,       /**< number of useful (non-obsolete) constraints to process */
    SCIP_SOL*          sol,                /**< primal solution that should be separated */
    SCIP_RESULT*       result              /**< pointer to store the result of the separation call */
    )
{
  SCIP_CALL( sepaDirectedSteinerCut(scip, conshdlr, conss, nconss, nusefulconss, sol, result) );
  return SCIP_OKAY;
}

//----------------------------------------
SCIP_RETCODE DCUTConstraintHandler::scip_enfolp(
    SCIP*              scip,               /**< SCIP data structure */
    SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
    SCIP_CONS**        conss,              /**< array of constraints to process */
    int                nconss,             /**< number of constraints to process */
    int                nusefulconss,       /**< number of useful (non-obsolete) constraints to process */
    SCIP_Bool          solinfeasible,      /**< was the solution already declared infeasible by a constraint handler? */
    SCIP_RESULT*       result              /**< pointer to store the result of the enforcing call */
    )
{
  *result = SCIP_FEASIBLE;
  for( int i = 0; i < nconss; ++i ) {
    SCIP_CONSDATA* consdata = SCIPconsGetData(conss[i]);
    assert(consdata != NULL);
    const STPData* data     = consdata->data;
    assert(data != NULL);

    if( findDirectedSteinerCut(scip, data, NULL) ) {
      *result = SCIP_INFEASIBLE;
      break;
    }
  }

  return SCIP_OKAY;
}

//----------------------------------------
SCIP_RETCODE DCUTConstraintHandler::scip_enfops(
    SCIP*              scip,               /**< SCIP data structure */
    SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
    SCIP_CONS**        conss,              /**< array of constraints to process */
    int                nconss,             /**< number of constraints to process */
    int                nusefulconss,       /**< number of useful (non-obsolete) constraints to process */
    SCIP_Bool          solinfeasible,      /**< was the solution already declared infeasible by a constraint handler? */
    SCIP_Bool          objinfeasible,      /**< is the solution infeasible anyway due to violating lower objective bound? */
    SCIP_RESULT*       result              /**< pointer to store the result of the enforcing call */
    )
{
  *result = SCIP_FEASIBLE;

  for( int i = 0; i < nconss; ++i ) {
    SCIP_CONSDATA* consdata = SCIPconsGetData(conss[i]);
    assert(consdata != NULL);
    const STPData* data     = consdata->data;
    assert(data != NULL);

    if( findDirectedSteinerCut(scip, data, NULL) ) {
      *result = SCIP_INFEASIBLE;
      break;
    }
  }
  return SCIP_OKAY;
}

//----------------------------------------
SCIP_RETCODE DCUTConstraintHandler::scip_check(
    SCIP*              scip,               /**< SCIP data structure */
    SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
    SCIP_CONS**        conss,              /**< array of constraints to process */
    int                nconss,             /**< number of constraints to process */
    SCIP_SOL*          sol,                /**< the solution to check feasibility for */
    SCIP_Bool          checkintegrality,   /**< has integrality to be checked? */
    SCIP_Bool          checklprows,        /**< have current LP rows to be checked? */
    SCIP_Bool          printreason,        /**< should the reason for the violation be printed? */
    SCIP_RESULT*       result              /**< pointer to store the result of the feasibility checking call */
    )
{
  *result = SCIP_FEASIBLE;

  for( int i = 0; i < nconss; ++i ) {
    SCIP_CONSDATA* consdata = SCIPconsGetData(conss[i]);
    assert(consdata != NULL);
    const STPData* data    = consdata->data;
    assert(data != NULL);

    if( findDirectedSteinerCut(scip, data, sol) ) {
      *result = SCIP_INFEASIBLE;
      if( printreason ) {
	SCIP_CALL( SCIPprintCons(scip, conss[i], NULL) );
	SCIPinfoMessage(scip, NULL, " Steiner cut violation\n");
      }
    }
  }

  return SCIP_OKAY;
}

//----------------------------------------
SCIP_RETCODE DCUTConstraintHandler::scip_lock(
    SCIP*              scip,               /**< SCIP data structure */
    SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
    SCIP_CONS*         cons,               /**< the constraint that should lock rounding of its variables, or NULL if the
					    *   constraint handler does not need constraints */
    int                nlockspos,          /**< no. of times, the roundings should be locked for the constraint */
    int                nlocksneg           /**< no. of times, the roundings should be locked for the constraint's negation */
    )
{
  SCIP_CONSDATA* consdata;

  consdata = SCIPconsGetData(cons);
  assert(consdata != NULL);

  const STPData* data = consdata->data;
  assert(data != NULL);

  for( int i = 0; i < data->graph().num_edges(); ++i ) {
    SCIP_CALL( SCIPaddVarLocks(scip, data->arc_var(i), nlocksneg, nlockspos) );
  }

  return SCIP_OKAY;
}

//----------------------------------------


SCIP_RETCODE SCIPcreateConsDirectedSteinerCutConstraint(
    SCIP*                 scip,               /**< SCIP data structure */
    SCIP_CONS**           cons,               /**< pointer to hold the created constraint */
    const char*           name,               /**< name of constraint */
    const STPData*        data,               /**< the underlying graph */
    SCIP_Bool             initial,            /**< should the LP relaxation of constraint be in the initial LP? */
    SCIP_Bool             separate,           /**< should the constraint be separated during LP processing? */
    SCIP_Bool             enforce,            /**< should the constraint be enforced during node processing? */
    SCIP_Bool             check,              /**< should the constraint be checked for feasibility? */
    SCIP_Bool             propagate,          /**< should the constraint be propagated during node processing? */
    SCIP_Bool             local,              /**< is constraint only valid locally? */
    SCIP_Bool             modifiable,         /**< is constraint modifiable (subject to column generation)? */
    SCIP_Bool             dynamic,            /**< is constraint dynamic? */
    SCIP_Bool             removable           /**< should the constraint be removed from the LP due to aging or cleanup? */
    )
{
  // Find the subtour constraint handler.
  SCIP_CONSHDLR* conshdlr = SCIPfindConshdlr( scip, HANDLER_NAME );
  if( conshdlr == NULL )
  {
    SCIPerrorMessage(HANDLER_NAME);
    SCIPerrorMessage(" not found\n");
    return SCIP_PLUGINNOTFOUND;
  }

  // Create constraint data.
  SCIP_CONSDATA* consdata = 0;
  SCIP_CALL( SCIPallocMemory( scip, &consdata ) );
  consdata->data = data;

  // Create constraint.
  SCIP_CALL( SCIPcreateCons( scip,
	cons,
	name,
	conshdlr,
	consdata,
	initial, separate, enforce, check, propagate,
	local, modifiable, dynamic, removable, FALSE) );

  return SCIP_OKAY;
}


//----------------------------------------
DCUTConstraintHandler::DCUTConstraintHandler(SCIP*  scip)
  : scip::ObjConshdlr( scip,
      HANDLER_NAME,      // name
      HANDLER_DESC,      // description
      1000000,           // separation priority
      -2000000,          // enforcing priority
      -2000000,          // check priority
      1,                 // separation frequency
      -1,                // propagation frequency (off)
      1,                 // eager frequency
      0,                 // max presolving rounds
      FALSE, FALSE, FALSE, TRUE, SCIP_PROPTIMING_BEFORELP )
{}


//----------------------------------------

DCUTConstraintHandler::~DCUTConstraintHandler()
{}


//----------------------------------------


SCIP_RETCODE DCUTConstraintHandler::scip_delete(
    SCIP*              scip,               /**< SCIP data structure */
    SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
    SCIP_CONS*         cons,               /**< the constraint belonging to the constraint data */
    SCIP_CONSDATA**    consdata            /**< pointer to the constraint data to free */
    )
{
  assert( consdata != NULL );
  SCIPfreeMemory( scip, consdata );
  return SCIP_OKAY;
}

//----------------------------------------

SCIP_RETCODE DCUTConstraintHandler::scip_trans(
    SCIP*              scip,               /**< SCIP data structure */
    SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
    SCIP_CONS*         sourcecons,         /**< source constraint to transform */
    SCIP_CONS**        targetcons          /**< pointer to store created target constraint */
    )
{
  SCIP_CONSDATA* sourcedata;
  SCIP_CONSDATA* targetdata;

  sourcedata = SCIPconsGetData(sourcecons);

  SCIP_CALL( SCIPallocMemory( scip, &targetdata ) );
  targetdata->data = sourcedata->data;

  /// Create target constraint.
  SCIP_CALL( SCIPcreateCons( scip, targetcons, SCIPconsGetName( sourcecons ), conshdlr, targetdata,
	SCIPconsIsInitial(sourcecons), SCIPconsIsSeparated(sourcecons),
	SCIPconsIsEnforced(sourcecons), SCIPconsIsChecked(sourcecons),
	SCIPconsIsPropagated(sourcecons), SCIPconsIsLocal(sourcecons),
	SCIPconsIsModifiable(sourcecons), SCIPconsIsDynamic(sourcecons),
	SCIPconsIsRemovable(sourcecons), SCIPconsIsStickingAtNode(sourcecons)) );

  return SCIP_OKAY;
}

//----------------------------------------

SCIP_RETCODE DCUTConstraintHandler::scip_print(
    SCIP*              scip,               /**< SCIP data structure */
    SCIP_CONSHDLR*     conshdlr,           /**< the constraint handler itself */
    SCIP_CONS*         cons,               /**< the constraint that should be displayed */
    FILE*              file                /**< the text file to store the information into */
    )
{
  SCIPinfoMessage( scip, file, "some cut ... \n" );

  return SCIP_OKAY;
}
