
const button_on = document.getElementsByClassName('title on')[0];
const button_off = document.getElementsByClassName('title off')[0];
const lamp = document.getElementsByClassName("lamp")[0];
const stat = document.getElementById('stat');
// const time_start = document.getElementById('input_time');
const time_catch = document.getElementById('catch_time');
const date_catch = document.getElementById('catch_date');

let time;

const url = 'http://192.168.1.1/';

(async function load() {
  const response = await fetch(url, { method: "PUT" });
  const ll = await response.json();
  dateObj = new Date(ll.start_programm * 1000);
  year = dateObj.getFullYear();
  month = dateObj.getMonth();
  day = dateObj.getDate();
  hours = dateObj.getHours();
  hours = dateObj.getHours();
  minutes = dateObj.getMinutes();
  seconds = dateObj.getSeconds();
  const form_date = `${day} : ${month} : ${year}`;
  const form_time = `${hours} : ${minutes} : ${seconds}`;
  date_catch.innerHTML = form_date;
  time_catch.innerHTML = form_time;

})();

button_on.addEventListener('click', async (e) => {
  const date = Math.floor(new Date().getTime() / 1000);
  const action = {
    "action": "on",
    "date": `${date}`
  };
  e.preventDefault();
  await fetch(url, {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify(action)
  });
  stat.innerHTML = 'Включено';
  lamp.classList.add('on');
});

button_off.addEventListener('click', async (e) => {
  const date = 0;
  const action = {
    "action": "off",
    "date": `${date}`
  };
  e.preventDefault();
  await fetch(url, {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify(action)
  });
  stat.innerHTML = 'Выключено'
  lamp.classList.remove('on');
})