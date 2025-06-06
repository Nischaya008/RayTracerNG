import { useState, useEffect } from 'react';
import { motion, AnimatePresence } from 'framer-motion';
import { Link } from 'react-scroll';
import { FaGithub, FaLinkedin, FaShareAlt } from 'react-icons/fa';

// Import components
import Features from './components/Features';
import TechStack from './components/TechStack';
import Architecture from './components/Architecture';
import Gallery from './components/Gallery';
import Download from './components/Download';
import Contribute from './components/Contribute';
import SocialLinksModal from './components/SocialLinksModal';
import AboutModal from './components/AboutModal';
import TextPressure from './components/TextPressure';

const App = () => {
  const [isScrolled, setIsScrolled] = useState(false);
  const [showSocialLinks, setShowSocialLinks] = useState(false);
  const [showAboutModal, setShowAboutModal] = useState(false);

  // Handle cursor teleportation on page load and refresh
  useEffect(() => {
    const teleportCursor = () => {
      // Get viewport dimensions
      const viewportWidth = window.innerWidth;
      const viewportHeight = window.innerHeight;
      
      // Create and dispatch a mousemove event to the extreme top right
      const event = new MouseEvent('mousemove', {
        clientX: viewportWidth - 1,
        clientY: 0,
        bubbles: true,
        cancelable: true,
        view: window
      });
      
      document.dispatchEvent(event);
    };

    // Call immediately on mount
    teleportCursor();

    // Also handle page refresh
    window.addEventListener('load', teleportCursor);

    return () => {
      window.removeEventListener('load', teleportCursor);
    };
  }, []);

  useEffect(() => {
    const handleScroll = () => {
      setIsScrolled(window.scrollY > 50);
    };

    window.addEventListener('scroll', handleScroll);

    return () => {
      window.removeEventListener('scroll', handleScroll);
    };
  }, []);

  return (
    <div className="min-h-screen bg-primary">
      {/* Navigation */}
      <nav className={`fixed w-full z-50 transition-all duration-300 ${
        isScrolled ? '-top-2 py-3' : 'top-4 py-6'
      }`}>
        <div className={`relative w-[95%] max-w-7xl mx-auto ${
          isScrolled ? 'glass backdrop-blur-xl bg-gradient-to-r from-primary/10 to-secondary/10 border-b border-white/5 shadow-2xl rounded-b-2xl py-6' : ''
        }`}>
          {/* Logo - Absolute left */}
          <div className="absolute left-8 top-1/2 -translate-y-1/2">
            <Link
              to="hero"
              smooth={true}
              duration={500}
              className="cursor-pointer"
            >
              <TextPressure
                text="RayTracerNG"
                flex={0.7}
                alpha={false}
                stroke={false}
                width={true}
                weight={0.7}
                italic={true}
                textColor="#1DCD9F"
                minFontSize={18}
                fontFamily="Inter"
                className="neon-text font-inter font-semibold tracking-wide"
              />
            </Link>
          </div>
          
          {/* Navigation - Absolute right */}
          <div className="absolute right-8 top-1/2 -translate-y-1/2 hidden md:flex items-center space-x-6">
            {['Features', 'Tech Stack', 'Architecture', 'Gallery', 'Download', 'Contribute'].map((item) => (
              <Link
                key={item}
                to={item.toLowerCase()}
                smooth={true}
                duration={500}
                className="text-white/70 hover:text-accent cursor-pointer transition-all duration-300 relative group px-2 py-1"
              >
                {item}
                <span className="absolute bottom-0 left-0 w-0 h-0.5 bg-accent/50 transition-all duration-300 group-hover:w-full"></span>
              </Link>
            ))}
          </div>
        </div>
      </nav>

      {/* Hero Section */}
      <section id="hero" className="min-h-screen flex items-center justify-center relative overflow-hidden">
        <div className="absolute inset-0 bg-gradient-to-b from-primary to-secondary opacity-50" />
        <div className="container relative z-10 text-center">
          {/* Container for Title and Subtitle */}
          <div className="flex flex-col items-center mb-8">
            <div className="w-full max-w-2xl mx-auto mb-4">
              <div className="w-full">
                <TextPressure
                  text="RayTracerNG"
                  flex={0.7}
                  alpha={false}
                  stroke={false}
                  width={true}
                  weight={0.7}
                  italic={true}
                  textColor="#1DCD9F"
                  minFontSize={48}
                  fontFamily="Inter"
                  className="neon-text font-inter font-semibold tracking-wide"
                />
              </div>
            </div>
            <motion.p
              className="text-xl md:text-2xl text-gray-300 mb-8"
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.8, delay: 0.2 }}
            >
              Real-time 2D Ray Tracing Engine with Modern OpenGL
            </motion.p>

            {/* Buttons */}
            <motion.div
              className="flex flex-row gap-4 justify-center items-center"
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.8, delay: 0.4 }}
            >
              <a href="https://github.com/Nischaya008/RayTracerNG" className="btn btn-primary" target="_blank" rel="noopener noreferrer">
                View on GitHub
              </a>
              <a href="#download" className="btn btn-secondary">
                Download Installer
              </a>
            </motion.div>
          </div>
        </div>
      </section>

      {/* Main Content */}
      <main>
        <Features />
        <TechStack />
        <Architecture />
        <Gallery />
        <Download />
        <Contribute />
      </main>

      {/* Footer */}
      <footer className="bg-secondary py-8">
        <div className="container mx-auto px-4">
          <div className="text-center">
            <div className="text-gray-400 text-sm">
              © 2025 RayTracerNG. All rights reserved.
            </div>
          </div>
        </div>
      </footer>

      {/* Social Links */}
      <SocialLinksModal onAboutClick={() => setShowAboutModal(true)} />

      {/* About Modal */}
      <AnimatePresence>
        {showAboutModal && (
          <AboutModal onClose={() => setShowAboutModal(false)} />
        )}
      </AnimatePresence>
    </div>
  );
};

export default App;
