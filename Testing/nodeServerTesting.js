const express = require("express");
const bodyParser = require("body-parser");
const ejs = require("ejs");
const engine = require('ejs-mate');

var app = express();
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({extended:true}));
app.engine('ejs',engine);
app.set('view engine', 'ejs')

let time="00:00.000"

app.get('/', (req,res)=>
{
    res.render("pages/lapcounterweb");
});

app.get('/update', (req,res)=>
{
    res.send(time);
})

app.post("/newrace", (req,res)=>
{
    console.log(req.body.player, req.body.laps)
    res.send("ok post");
})

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

    time = min + ":" + sec + "." + milli;
    console.log(time)
    
},5000)