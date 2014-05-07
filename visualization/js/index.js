var margin = {top: 20, right: 40, bottom: 70, left: 40},
    width = 1024 - margin.left - margin.right,
    height = 768 - margin.top - margin.bottom;

//var x = d3.scale.ordinal()
//    .rangeRoundBands([0, width], .1);
/*
var x = d3.time.scale()
  .domain([new Date(1399460610),new Date(1399460950)])  
  .range([0, width])
  .ticks(10);
*/
/*var x = d3.time.scale()
    .domain([1399460610,1399460950])
    .range([0, width]);
*/
var x = d3.scale.linear()
    .range([0, width]);

var m1 = d3.scale.linear()
    .range([height, 0]);

var m2 = d3.scale.linear()
    .range([height, 0]);

var m3 = d3.scale.linear()
    .range([height, 0]);

var xAxis = d3.svg.axis()
    .scale(x)
    .orient("bottom")
    .ticks(14, "");

var m1Axis = d3.svg.axis()
    .scale(m1)
    .orient("left")
    .ticks(10, "");

var m2Axis = d3.svg.axis()
    .scale(m2)
    .orient("right")
    .ticks(10, "");

var m3Axis = d3.svg.axis()
    .scale(m3)
    .orient("right")
    .ticks(10, "");

var svg = d3.select("body").append("svg")
    .attr("width", width + margin.left + margin.right)
    .attr("height", height + margin.top + margin.bottom)
  .append("g")
    .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

var index=getParameterByName('index');
var metric = getParameterByName('metric');
var metric2=getParameterByName('metric2');
var metric3=getParameterByName('metric3');
var from=getParameterByName('from');
var to=getParameterByName('to');
console.log(metric);
//var xyz = jQuery.url.param("xyz");
/*//This is the original part that was getting the info from a tsv
d3.tsv("data.tsv", type, function(error, data) {
  x.domain(data.map(function(d) {  console.log(d.letter); return d.letter; }));
  y.domain([0, d3.max(data, function(d) { console.log(d.frequency);return d.frequency; })]);
*/
//This is the part i added that grabs the data from a json file
d3.json('/executions/'+index,function(error,jsondata) {
  if (error) return console.warn(error);
  data = jsondata;
 // data.forEach(function(d){ d.Timestamp = new Date(d.Timestamp * 1000) });

  x.domain([d3.min(data, function(d) { return d["Timestamp"]; }), d3.max(data, function(d) { return d["Timestamp"]; })]);


  m1.domain([0, d3.max(data, function(d) { 
                                          //console.log(d[metric]); 
                                          return d[metric]; })]);
  m2.domain([0, d3.max(data, function(d) { 
                                          //console.log(d[metric2]); 
                                          return d[metric2]; })]);
  m3.domain([0, d3.max(data, function(d) { 
                                          //console.log(d[metric2]); 
                                          return d[metric3]; })]);
//Completion of my part, now let d3 do its magic 
  svg.append("g")
      .attr("class", "x axis")
      .attr("transform", "translate(0," + height + ")")
      .call(xAxis)
      .selectAll("text")  
            .style("text-anchor", "end")
            .attr("dx", "-.8em")
            .attr("dy", ".15em")
            .attr("transform", function(d) {
                return "rotate(-65)" 
              });

  svg.append("g")
      .attr("class", "m1 axis")
      .call(m1Axis)
    .append("text")
      .attr("transform", "rotate(-90)")
      .attr("y", -15)
      .attr("dy", ".71em")
      .style("text-anchor", "end")
      .text(metric);
            
   svg.append("g")
      .attr("class", "m2 axis")
      .call(m2Axis)
    .append("text")
      .attr("transform", "rotate(-90)")
      .attr("y", 6)
      .attr("dy", ".71em")
      .style("text-anchor", "end")
      .text(metric2);

   svg.append("g")
      .attr("class", "m3 axis")
      .attr("transform", "translate("+ width +",0)")
      .call(m3Axis)
    .append("text")
      .attr("transform", "rotate(-90)")
      .attr("y", 0)
      .attr("dy", ".71em")
      .style("text-anchor", "end")
      .text(metric3);
/*
  svg.selectAll(".bar")
      .data(data)
    .enter().append("rect")
      .attr("class", "bar")
      .attr("x", function(d) { return x(d.Timestamp); })
      .attr("width", x.rangeBand())
      .attr("y", function(d) { return m1(d[metric]); })
      .attr("height", function(d) { return height - m1(d[metric]); });

/*  svg.selectAll(".line")
      .data(data)
    .enter().append("rect")
      .attr("class", "line")
      .attr("x", function(d) { return x(d.Timestamp); })
      .attr("width", x.rangeBand())
      .attr("y", function(d) { return z(d["User_CPU"]); })
      .attr("height", function(d) { return height - z(d["User_CPU"]); });
});
*/
/*
var circles = svg.selectAll("circle")
                          .data(data)
                          .enter()
                          .append("circle");

var circleAttributes = circles
                       .attr("cx", function (d) { return x(d.Timestamp)+x.rangeBand()/2; })
                       .attr("cy", function (d) { return m2(d["User_CPU"]); })
                       .attr("r", x.rangeBand()/5)
                       .style("fill", "red");
*/



//createLineGraph("User_CPU","red");
createLineGraph(metric,"red",m1);
createLineGraph(metric2,"blue",m2);
createLineGraph(metric3,"green",m3);

});


function getDate(d) {
    return new Date(d.Timestamp);
}

function getParameterByName(name) {
    name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
    var regex = new RegExp("[\\?&]" + name + "=([^&#]*)"),
        results = regex.exec(location.search);
    return results == null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
}

function createLineGraph(variable,color,m){
var lineFunction = d3.svg.line()
                         .x(function(d) {  return x(d.Timestamp); })
                          .y(function(d) {  return m(d[variable]); })
                         .interpolate("linear");               

var lineGraph = svg.append("path")
                            .attr("d", lineFunction(data,variable) )
                            .attr("stroke", color)
                            .attr("stroke-width", 2)
                            .attr("fill", "none");
}
/*
function type(d) {
  d[metric] = +d[metric];
  return d;
}*/
