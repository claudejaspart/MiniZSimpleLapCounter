const express = require("express");
const bodyParser = require("body-parser");
const ejs = require("ejs");
const engine = require('ejs-mate');

var app = express();
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({extended:true}));
app.engine('ejs',engine);
app.set('view engine', 'ejs')

let time="00:00.000";
let race_started = false;

// ir tweaking options
let minValue= 0;
let maxValue=0;
let irCut = 0;


app.get('/', (req,res)=>
{
    res.render("pages/lapcounterweb");
});

app.get('/update', (req,res)=>
{
    res.send(time);
});

app.get('/reset', (req,res)=>
{
    race_started=false;
    console.log("reset ok");
    res.sendStatus(200);
});

app.get('/start', (req,res)=>
{
    race_started=true;
    console.log("start ok");
    res.sendStatus(200);
});

app.get('/stop', (req,res)=>
{
    race_started=false;
    console.log("stop ok");
    res.sendStatus(200);
});

// measure ir settings
app.get('/measure', (req,res)=>
{
    console.log("measuring");
    res.sendStatus(200);
});


// retrieve ir settings
app.get('/retrieve', (req,res)=>
{
    console.log("retrieve data");
    let data = {
        "minValues":"100,150,130,160,130",
        "maxValues":"220,210,210,220,210",
        "irCutValue" : "200"
    }
    res.json(data);
});

// set ir settings
app.get('/setir', (req,res)=>
{
    console.log(req.query.ircut);
    irCut = req.query.ircut;
    res.sendStatus(200);
});



app.listen(5000, (err)=>
{
    if (err) throw err;
    console.log("Server is running on port 5000");
});

// simulateur de chrono
setInterval(()=>
{
    let min = parseInt(Math.random() * 59);
    let sec = parseInt(Math.random() * 60);
    let milli = parseInt(Math.random() * 999);
    if (min < 10) min = "0" + min;
    if (sec < 10) sec = "0" + sec;
    if (milli < 10 ) 
        milli = "00" + milli;
    else if (milli < 100)
        milli = "0" + milli;

    time = min + ":" + sec + ":" + milli;
    console.log(time)
    
},5000)