/*
 * GET metrics name.
 */
exports.metrics = function(db) {
   return function(req, res) {
      console.log("From tools......................> "+req.params.msg);
   }
};

