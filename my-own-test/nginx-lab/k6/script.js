import http from 'k6/http';
import { check, sleep } from 'k6';

const BASE = __ENV.TARGET || 'http://localhost:8080';

export const options = {
  scenarios: {
    step_rps: {
      executor: 'ramping-arrival-rate',
      startRate: 10000,      // 시작 RPS
      timeUnit: '1s',
      preAllocatedVUs: 1500, // 초기 VU 풀
      maxVUs: 6000,          // 상한 (필요시 더 키우세요)
      stages: [
        { target: 10000, duration: '20s' }, // 10k RPS 유지
        { target: 15000, duration: '20s' }, // 15k RPS로 상승
        { target: 20000, duration: '20s' }, // 20k RPS로 상승
      ],
      gracefulStop: '30s',
    },
  },
  thresholds: {
    http_req_failed: ['rate<0.01'],
    http_req_duration: ['p(99)<50'], // 20k RPS에선 다소 공격적일 수 있음
  },
};

export default function () {
  const res = http.get(`${BASE}/`);
  check(res, { 'status 200': (r) => r.status === 200 });
  sleep(0.001);
}

