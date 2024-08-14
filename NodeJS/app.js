const express = require('express');
const app = express();

app.get('/:num1/:num2', (req, res) => {
    const num1 = parseInt(req.params.num1, 10);
    const num2 = parseInt(req.params.num2, 10);
    const result = num1 + num2;
    res.send({ result: result });
});

const port = 4000;
app.listen(port, () => {
    console.log(`Server running on port ${port}`);
});