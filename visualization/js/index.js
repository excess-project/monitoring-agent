var margin = {top: 20, right: 20, bottom: 30, left: 40},
    width = 960 - margin.left - margin.right,
    height = 500 - margin.top - margin.bottom;

var x = d3.scale.ordinal()
    .rangeRoundBands([0, width], .1);

var y = d3.scale.linear()
    .range([height, 0]);

var xAxis = d3.svg.axis()
    .scale(x)
    .orient("bottom");

var yAxis = d3.svg.axis()
    .scale(y)
    .orient("left")
    .ticks(10, "");

var svg = d3.select("body").append("svg")
    .attr("width", width + margin.left + margin.right)
    .attr("height", height + margin.top + margin.bottom)
  .append("g")
    .attr("transform", "translate(" + margin.left + "," + margin.top + ")");
/*//This is the original part that was getting the info from a tsv
d3.tsv("data.tsv", type, function(error, data) {
  x.domain(data.map(function(d) {  console.log(d.letter); return d.letter; }));
  y.domain([0, d3.max(data, function(d) { console.log(d.frequency);return d.frequency; })]);
*/
//This is the part i added that grabs the data from a json file
d3.json("http://localhost:3000/executions/kdolkas7",function(error,jsondata) {
  if (error) return console.warn(error);
  data = jsondata;
  metric = "Memory";
  
   
  console.log(JSON.stringify(data));

  x.domain(data.map(function(d) { console.log(d.Timestamp);return d.Timestamp;  }));
 // y.domain(jsondata.frequency.map(function(d) { return d.frequency; }));
  y.domain([0, d3.max(data, function(d) { console.log(d[metric]); return d[metric]; })]);

//Completion of my part, now let d3 do its magic 

  svg.append("g")
      .attr("class", "x axis")
      .attr("transform", "translate(0," + height + ")")
      .call(xAxis);

  svg.append("g")
      .attr("class", "y axis")
      .call(yAxis)
    .append("text")
      .attr("transform", "rotate(-90)")
      .attr("y", 6)
      .attr("dy", ".71em")
      .style("text-anchor", "end")
      .text(metric);

  svg.selectAll(".bar")
      .data(data)
    .enter().append("rect")
      .attr("class", "bar")
      .attr("x", function(d) { return x(d.Timestamp); })
      .attr("width", x.rangeBand())
      .attr("y", function(d) { return y(d[metric]); })
      .attr("height", function(d) { return height - y(d[metric]); });

});

function type(d) {
  d.User_CPU = +d.User_CPU;
  return d;
}
