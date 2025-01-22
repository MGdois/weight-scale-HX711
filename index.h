const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<style>
.card1{
    max-width: 400px;
     min-height: 250px;
     background: #02b875;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
     text-align: center; /* Centraliza o conteúdo dentro do card */
}
</style>
<body>
<style>
.card2{
    max-width: 400px;
     min-height: 100px;
     background: #FF6347;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
     text-align: center; /* Centraliza o conteúdo dentro do card */
}
</style>
<body>


<div class="card1">
  <h3>MONITORAMENTO: </h3><br>
  <h2 id="mensagemCliente">Carregando...</h2>
 
</div>

<div class="card2">
  <h1 id="novaMensagem">...</h1>
</div>

<script>
setInterval(function() {
  // Call a function repetatively with 2 Second interval
    getData("readADC", "mensagemCliente"); // Atualiza o monitoramento do gás
}, 1000); 

// Atualiza o status do sistema
setInterval(function() {
    getData("readNovaMensagem", "novaMensagem"); // Atualiza o status do sistema
}, 1000); // Atualização a cada 2 segundos

// Função genérica para obter dados do servidor
function getData(route, elementId) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById(elementId).innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", route, true);
    xhttp.send();
}
</script>
</body>
</html>
)=====";
