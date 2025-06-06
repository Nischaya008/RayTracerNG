/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors: {
        primary: '#000000',
        secondary: '#222222',
        accent: '#1DCD9F',
        'accent-dark': '#169976',
      },
      fontFamily: {
        sans: ['Inter var', 'Inter', 'sans-serif'],
        inter: ['Inter var', 'Inter', 'sans-serif'],
        display: ['Poppins', 'sans-serif'],
      },
      animation: {
        'glow': 'glow 2s ease-in-out infinite alternate',
        'float': 'float 6s ease-in-out infinite',
      },
      keyframes: {
        glow: {
          '0%': { boxShadow: '0 0 5px #1DCD9F, 0 0 10px #1DCD9F, 0 0 15px #1DCD9F' },
          '100%': { boxShadow: '0 0 10px #1DCD9F, 0 0 20px #1DCD9F, 0 0 30px #1DCD9F' },
        },
        float: {
          '0%, 100%': { transform: 'translateY(0)' },
          '50%': { transform: 'translateY(-20px)' },
        },
      },
    },
  },
  plugins: [],
} 
