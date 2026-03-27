import Chart from 'https://cdn.jsdelivr.net/npm/chart.js@4.4.0/dist/chart.umd.js';

export function createIMUChart() {
  const ctx = document.getElementById('imuChart');

  const chart = new Chart(ctx, {
    type: 'line',
    data: {
      labels: [],
      datasets: [
        { label: 'ax', data: [], borderColor: 'red' },
        { label: 'ay', data: [], borderColor: 'green' },
        { label: 'az', data: [], borderColor: 'blue' }
      ]
    },
    options: {
      animation: false,
      responsive: true,
      scales: {
        x: { display: false }
      }
    }
  });

  return chart;
}