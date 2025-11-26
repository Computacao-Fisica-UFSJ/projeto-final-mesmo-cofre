# 🐾 Projeto Yummy

## 🔗 Referência do Projeto
[Automatic Arduino Powered Pet Feeder](https://www.instructables.com/Automatic-Arduino-Powered-Pet-Feeder/)

---

## 🧱 Materiais Auxiliares
- Impressora 3D  
- Balança digital  

---

## 🧰 Materiais

### 🔌 Componentes 
- **1x** Servo RC normal — tamanho padrão  
- **1x** Servo RC contínuo — padrão ou adaptado  
- **1x** Peças impressas em 3D para a broca  
- **1x** Tubo de PVC em T de **1 ½"**  
- **1x** Arduino Uno  
- **1x** Encoder rotativo KY-040 integrado *(se precisar iremos adaptar)*  
- **1x** LCD 16x2 compatível com **HD44780**  
- **1x** Pequeno interruptor de botão (sem trava)  
- **1x** Reservatório de ração *(preferencialmente com tampa hermética para evitar “atividades criminosas” de gatos 😹)*  
- **1x** Caixa de montagem  
- **1x** Fonte de alimentação **12V / 500mA** (ou superior, para segurança dos servos)  
- **1x** Módulo Wi-Fi *(modelo ainda a definir)*  
- **1x** Balança

---

## ⚙️ Observações
- As peças 3D e o tubo de PVC formam o mecanismo de liberação da ração.  
- A **balança** será usada para calibrar a quantidade de ração liberada.


## Evolução do Projeto

## Etapas realizadas até a aula do dia 19/11:
- Abertura da balança: tentamos entender como a informação é passada pelos componentes da balança e vimos que seria necessário o uso de um módulo conversor para o sensor de peso (HX711).
- Solda dos fios que saem do sensor de peso da balança na placa HX711, e dos fios que saem da mesma placa para serem enviados para a placa Arduíno.
- Com a conexão do sensor de peso ao arduíno, realizamos a calibração da balança com um código.
- Conexão do servo ao projeto: esse servo controlará o agitador de ração. Produzimos um código inicial, o qual aguarda o sinal de botão para agitar a ração, até que um certo peso na balança é atingido. 

## Aula 25/11:
- Análise de motores possíveis para controlar a rosca que libera a ração. Escolhemos o Micro Motor DC N20 e tentamos conectá-lo à uma ponte H para dividir a alimentação. Contudo, tivemos problema com a fonte que utilizamos e resolvemos conectar o motor diretamente ao arduíno. Incluímos essa parte no código, ativando o motor depois que o servo do agitador para, até que o peso seja atingido.


---

